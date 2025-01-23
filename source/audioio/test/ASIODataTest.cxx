#include "gtest/gtest.h"

#include "audioio/inc/ASIOData.h"
#include "engine/inc/FormatTypeFromFloat.h"
#include "engine/inc/FIRFilter.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo16BitLSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7fff =  32767 <-  65534 =  65534.0 / 65534.0 =  1.0
		 0.8, //  1:  0x6666 =  26214 <-  52428 =  52427.2 / 65534.0 =  0.8 ( 52427.2  52426=1.2  52428=0.8)
		 0.6, //  2:  0x4ccc =  19660 <-  39320 =  39320.4 / 65534.0 =  0.6 ( 39320.4  39320=0.4  39322=1.6)
		 0.4, //  3:  0x3333 =  13107 <-  26214 =  26213.6 / 65534.0 =  0.4 ( 26213.6  26212=1.6  26214=0.4)
		 0.2, //  4:  0x1999 =   6553 <-  13106 =  13106.8 / 65534.0 =  0.2 ( 13106.8  13106=0.8  13108=1.2)
		-0.2, //  5:  0xe666 =  -6554 <- -13108 = -13107.2 / 65536.0 = -0.2 (-13107.2 -13108=0.8 -13106=1.2)
		-0.4, //  6:  0xcccd = -13107 <- -26214 = -26214.4 / 65536.0 = -0.4 (-26214.4 -26214=0.4 -26216=1.6)
		-0.6, //  7:  0xb333 = -19661 <- -39322 = -39321.6 / 65536.0 = -0.6 (-39321.6 -39320=1.6 -39322=0.4)
		-0.8, //  8:  0x999a = -26214 <- -52428 = -52428.8 / 65536.0 = -0.8 (-52428.8 -52428=0.8 -52430=1.2)
		-1.0, //  9:  0x8000 = -32768 <- -65536 = -65536.0 / 65536.0 = -1.0
		 1.2, // 10:  0x7fff =  32767 =  1.2
		-1.2  // 11:  0x8000 = -32768 = -1.2
	};
#endif
	
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

	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTInt16LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamples,24));
}

TEST(ASIOData,sampleTo16BitLSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7fff =  32767 <-  65534 =  65534.0 / 65534.0 =  1.0
		 0.8, //  1:  0x6666 =  26214 <-  52428 =  52427.2 / 65534.0 =  0.8 ( 52427.2  52426=1.2  52428=0.8)
		 0.6, //  2:  0x4ccc =  19660 <-  39320 =  39320.4 / 65534.0 =  0.6 ( 39320.4  39320=0.4  39322=1.6)
		 0.4, //  3:  0x3333 =  13107 <-  26214 =  26213.6 / 65534.0 =  0.4 ( 26213.6  26212=1.6  26214=0.4)
		 0.2, //  4:  0x1999 =   6553 <-  13106 =  13106.8 / 65534.0 =  0.2 ( 13106.8  13106=0.8  13108=1.2)
		-0.2, //  5:  0xe666 =  -6554 <- -13108 = -13107.2 / 65536.0 = -0.2 (-13107.2 -13108=0.8 -13106=1.2)
		-0.4, //  6:  0xcccd = -13107 <- -26214 = -26214.4 / 65536.0 = -0.4 (-26214.4 -26214=0.4 -26216=1.6)
		-0.6, //  7:  0xb333 = -19661 <- -39322 = -39321.6 / 65536.0 = -0.6 (-39321.6 -39320=1.6 -39322=0.4)
		-0.8, //  8:  0x999a = -26214 <- -52428 = -52428.8 / 65536.0 = -0.8 (-52428.8 -52428=0.8 -52430=1.2)
		-1.0, //  9:  0x8000 = -32768 <- -65536 = -65536.0 / 65536.0 = -1.0
		 1.2, // 10:  0x7fff =  32767 =  1.2
		-1.2  // 11:  0x8000 = -32768 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[12] = {
		0xff, 0x7f, // 0
		0xcc, 0x4c, // 2
		0x99, 0x19, // 4
		0xcd, 0xcc, // 6
		0x9a, 0x99, // 8
		0xff, 0x7f  // 10
	};
	
	const tubyte c_expectIntegerSamplesB[12] = {
		0x66, 0x66, // 1
		0x33, 0x33, // 3
		0x66, 0xe6, // 5
		0x33, 0xb3, // 7
		0x00, 0x80, // 9
		0x00, 0x80  // 11
	};

	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt16LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,12));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,12));
}

TEST(ASIOData,sampleTo16BitLSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7fff =  32767 <-  65534 =  65534.0 / 65534.0 =  1.0
		 0.8, //  1:  0x6666 =  26214 <-  52428 =  52427.2 / 65534.0 =  0.8 ( 52427.2  52426=1.2  52428=0.8)
		 0.6, //  2:  0x4ccc =  19660 <-  39320 =  39320.4 / 65534.0 =  0.6 ( 39320.4  39320=0.4  39322=1.6)
		 0.4, //  3:  0x3333 =  13107 <-  26214 =  26213.6 / 65534.0 =  0.4 ( 26213.6  26212=1.6  26214=0.4)
		 0.2, //  4:  0x1999 =   6553 <-  13106 =  13106.8 / 65534.0 =  0.2 ( 13106.8  13106=0.8  13108=1.2)
		-0.2, //  5:  0xe666 =  -6554 <- -13108 = -13107.2 / 65536.0 = -0.2 (-13107.2 -13108=0.8 -13106=1.2)
		-0.4, //  6:  0xcccd = -13107 <- -26214 = -26214.4 / 65536.0 = -0.4 (-26214.4 -26214=0.4 -26216=1.6)
		-0.6, //  7:  0xb333 = -19661 <- -39322 = -39321.6 / 65536.0 = -0.6 (-39321.6 -39320=1.6 -39322=0.4)
		-0.8, //  8:  0x999a = -26214 <- -52428 = -52428.8 / 65536.0 = -0.8 (-52428.8 -52428=0.8 -52430=1.2)
		-1.0, //  9:  0x8000 = -32768 <- -65536 = -65536.0 / 65536.0 = -1.0
		 1.2, // 10:  0x7fff =  32767 =  1.2
		-1.2  // 11:  0x8000 = -32768 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[6] = {
		0xff, 0x7f, // 0
		0x99, 0x19, // 4
		0x9a, 0x99  // 8
	};
	
	const tubyte c_expectIntegerSamplesB[6] = {
		0x66, 0x66, // 1
		0x66, 0xe6, // 5
		0x00, 0x80  // 9
	};
	
	const tubyte c_expectIntegerSamplesC[6] = {
		0xcc, 0x4c, // 2
		0xcd, 0xcc, // 6
		0xff, 0x7f  // 10
	};
	
	const tubyte c_expectIntegerSamplesD[6] = {
		0x33, 0x33, // 3
		0x33, 0xb3, // 7
		0x00, 0x80  // 11
	};
	
	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTInt16LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,6));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,6));
	ASSERT_EQ(0,memcmp(data.asioData(2,0),c_expectIntegerSamplesC,6));
	ASSERT_EQ(0,memcmp(data.asioData(3,0),c_expectIntegerSamplesD,6));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo16BitLSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[12] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000,
		0x7fff, 0x8000
	};
	
	const tubyte c_expectIntegerSamplesA[12] = {
		0xff, 0x7f, // 0
		0xcc, 0x4c, // 2
		0x99, 0x19, // 4
		0xcd, 0xcc, // 6
		0x9a, 0x99, // 8
		0xff, 0x7f  // 10
	};
	
	const tubyte c_expectIntegerSamplesB[12] = {
		0x66, 0x66, // 1
		0x33, 0x33, // 3
		0x66, 0xe6, // 5
		0x33, 0xb3, // 7
		0x00, 0x80, // 9
		0x00, 0x80  // 11
	};

	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt16LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(tint16));
	part.length() = 6;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,12));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,12));
}

TEST(ASIOData,sampleTo16BitLSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[14] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000,
		0x00000080, 0x0000007f, 0x00000000, 0xffffff80
	};
	
	const tubyte c_expectIntegerSamplesA[14] = {
		0xff, 0x7f,
		0xcd, 0x4c,
		0x9a, 0x19,
		0xcd, 0xcc, 
		0x9a, 0x99, 
		0x01, 0x00,
		0x00, 0x00
	};
	const tubyte c_expectIntegerSamplesB[14] = {
		0x66, 0x66, 
		0x33, 0x33, 
		0x66, 0xe6, 
		0x33, 0xb3,
		0x00, 0x80,
		0x00, 0x00, 
		0x00, 0x00
	};

	ASIOData data(7,2,2);
	data.setSampleType(ASIOSTInt16LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,14 * sizeof(tint32));
	part.length() = 7;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,14));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,14));
}

TEST(ASIOData,sampleTo16BitLSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[14] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00008000, 0x00007fff, 0x00000000, 0xffff8000
	};
	
	const tubyte c_expectIntegerSamplesA[14] = {
		0xff, 0x7f,
		0xcd, 0x4c,
		0x9a, 0x19,
		0xcd, 0xcc, 
		0x9a, 0x99, 
		0x01, 0x00,
		0x00, 0x00
	};
	const tubyte c_expectIntegerSamplesB[14] = {
		0x66, 0x66, 
		0x33, 0x33, 
		0x66, 0xe6, 
		0x33, 0xb3,
		0x00, 0x80,
		0x00, 0x00, 
		0x00, 0x00
	};

	ASIOData data(7,2,2);
	data.setSampleType(ASIOSTInt16LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,14 * sizeof(tint32));
	part.length() = 7;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,14));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,14));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo16BitMSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[12] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000,
		0x7fff, 0x8000
	};
	
	const tubyte c_expectIntegerSamplesA[12] = {
		0x7f, 0xff, // 0
		0x4c, 0xcc, // 2
		0x19, 0x99, // 4
		0xcc, 0xcd, // 6
		0x99, 0x9a, // 8
		0x7f, 0xff  // 10
	};
	
	const tubyte c_expectIntegerSamplesB[12] = {
		0x66, 0x66, // 1
		0x33, 0x33, // 3
		0xe6, 0x66, // 5
		0xb3, 0x33, // 7
		0x80, 0x00, // 9
		0x80, 0x00  // 11
	};

	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt16MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(tint16));
	part.length() = 6;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,12));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,12));
}

TEST(ASIOData,sampleTo16BitMSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[14] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000,
		0x00000080, 0x0000007f, 0x00000000, 0xffffff80
	};
	
	const tubyte c_expectIntegerSamplesA[14] = {
		0x7f, 0xff,
		0x4c, 0xcd,
		0x19, 0x9a,
		0xcc, 0xcd,
		0x99, 0x9a,
		0x00, 0x01,
		0x00, 0x00
	};
	const tubyte c_expectIntegerSamplesB[14] = {
		0x66, 0x66, 
		0x33, 0x33,
		0xe6, 0x66,
		0xb3, 0x33,
		0x80, 0x00,
		0x00, 0x00,
		0x00, 0x00
	};

	ASIOData data(7,2,2);
	data.setSampleType(ASIOSTInt16MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,14 * sizeof(tint32));
	part.length() = 7;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,14));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,14));
}

TEST(ASIOData,sampleTo16BitMSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[14] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00008000, 0x00007fff, 0x00000000, 0xffff8000
	};

	const tubyte c_expectIntegerSamplesA[14] = {
		0x7f, 0xff,
		0x4c, 0xcd,
		0x19, 0x9a,
		0xcc, 0xcd,
		0x99, 0x9a,
		0x00, 0x01,
		0x00, 0x00
	};
	const tubyte c_expectIntegerSamplesB[14] = {
		0x66, 0x66, 
		0x33, 0x33,
		0xe6, 0x66,
		0xb3, 0x33,
		0x80, 0x00,
		0x00, 0x00,
		0x00, 0x00
	};

	ASIOData data(7,2,2);
	data.setSampleType(ASIOSTInt16MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,14 * sizeof(tint32));
	part.length() = 7;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,14));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,14));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo16BitMSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7fff =  32767 <-  65534 =  65534.0 / 65534.0 =  1.0
		 0.8, //  1:  0x6666 =  26214 <-  52428 =  52427.2 / 65534.0 =  0.8 ( 52427.2  52426=1.2  52428=0.8)
		 0.6, //  2:  0x4ccc =  19660 <-  39320 =  39320.4 / 65534.0 =  0.6 ( 39320.4  39320=0.4  39322=1.6)
		 0.4, //  3:  0x3333 =  13107 <-  26214 =  26213.6 / 65534.0 =  0.4 ( 26213.6  26212=1.6  26214=0.4)
		 0.2, //  4:  0x1999 =   6553 <-  13106 =  13106.8 / 65534.0 =  0.2 ( 13106.8  13106=0.8  13108=1.2)
		-0.2, //  5:  0xe666 =  -6554 <- -13108 = -13107.2 / 65536.0 = -0.2 (-13107.2 -13108=0.8 -13106=1.2)
		-0.4, //  6:  0xcccd = -13107 <- -26214 = -26214.4 / 65536.0 = -0.4 (-26214.4 -26214=0.4 -26216=1.6)
		-0.6, //  7:  0xb333 = -19661 <- -39322 = -39321.6 / 65536.0 = -0.6 (-39321.6 -39320=1.6 -39322=0.4)
		-0.8, //  8:  0x999a = -26214 <- -52428 = -52428.8 / 65536.0 = -0.8 (-52428.8 -52428=0.8 -52430=1.2)
		-1.0, //  9:  0x8000 = -32768 <- -65536 = -65536.0 / 65536.0 = -1.0
		 1.2, // 10:  0x7fff =  32767 =  1.2
		-1.2  // 11:  0x8000 = -32768 = -1.2
	};
#endif

	const tubyte c_expectIntegerSamples[24] = {
		0x7f, 0xff, // 0
		0x66, 0x66, // 1
		0x4c, 0xcc, // 2
		0x33, 0x33, // 3
		0x19, 0x99, // 4
		0xe6, 0x66, // 5
		0xcc, 0xcd, // 6
		0xb3, 0x33, // 7
		0x99, 0x9a, // 8
		0x80, 0x00, // 9
		0x7f, 0xff, // 10
		0x80, 0x00  // 11
	};

	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTInt16MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamples,24));
}

TEST(ASIOData,sampleTo16BitMSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7fff =  32767 <-  65534 =  65534.0 / 65534.0 =  1.0
		 0.8, //  1:  0x6666 =  26214 <-  52428 =  52427.2 / 65534.0 =  0.8 ( 52427.2  52426=1.2  52428=0.8)
		 0.6, //  2:  0x4ccc =  19660 <-  39320 =  39320.4 / 65534.0 =  0.6 ( 39320.4  39320=0.4  39322=1.6)
		 0.4, //  3:  0x3333 =  13107 <-  26214 =  26213.6 / 65534.0 =  0.4 ( 26213.6  26212=1.6  26214=0.4)
		 0.2, //  4:  0x1999 =   6553 <-  13106 =  13106.8 / 65534.0 =  0.2 ( 13106.8  13106=0.8  13108=1.2)
		-0.2, //  5:  0xe666 =  -6554 <- -13108 = -13107.2 / 65536.0 = -0.2 (-13107.2 -13108=0.8 -13106=1.2)
		-0.4, //  6:  0xcccd = -13107 <- -26214 = -26214.4 / 65536.0 = -0.4 (-26214.4 -26214=0.4 -26216=1.6)
		-0.6, //  7:  0xb333 = -19661 <- -39322 = -39321.6 / 65536.0 = -0.6 (-39321.6 -39320=1.6 -39322=0.4)
		-0.8, //  8:  0x999a = -26214 <- -52428 = -52428.8 / 65536.0 = -0.8 (-52428.8 -52428=0.8 -52430=1.2)
		-1.0, //  9:  0x8000 = -32768 <- -65536 = -65536.0 / 65536.0 = -1.0
		 1.2, // 10:  0x7fff =  32767 =  1.2
		-1.2  // 11:  0x8000 = -32768 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[12] = {
		0x7f, 0xff, // 0
		0x4c, 0xcc, // 2
		0x19, 0x99, // 4
		0xcc, 0xcd, // 6
		0x99, 0x9a, // 8
		0x7f, 0xff, // 10
	};

	const tubyte c_expectIntegerSamplesB[12] = {
		0x66, 0x66, // 1
		0x33, 0x33, // 3
		0xe6, 0x66, // 5
		0xb3, 0x33, // 7
		0x80, 0x00, // 9
		0x80, 0x00  // 11
	};
	
	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt16MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,12));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,12));
}

TEST(ASIOData,sampleTo16BitMSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7fff =  32767 <-  65534 =  65534.0 / 65534.0 =  1.0
		 0.8, //  1:  0x6666 =  26214 <-  52428 =  52427.2 / 65534.0 =  0.8 ( 52427.2  52426=1.2  52428=0.8)
		 0.6, //  2:  0x4ccc =  19660 <-  39320 =  39320.4 / 65534.0 =  0.6 ( 39320.4  39320=0.4  39322=1.6)
		 0.4, //  3:  0x3333 =  13107 <-  26214 =  26213.6 / 65534.0 =  0.4 ( 26213.6  26212=1.6  26214=0.4)
		 0.2, //  4:  0x1999 =   6553 <-  13106 =  13106.8 / 65534.0 =  0.2 ( 13106.8  13106=0.8  13108=1.2)
		-0.2, //  5:  0xe666 =  -6554 <- -13108 = -13107.2 / 65536.0 = -0.2 (-13107.2 -13108=0.8 -13106=1.2)
		-0.4, //  6:  0xcccd = -13107 <- -26214 = -26214.4 / 65536.0 = -0.4 (-26214.4 -26214=0.4 -26216=1.6)
		-0.6, //  7:  0xb333 = -19661 <- -39322 = -39321.6 / 65536.0 = -0.6 (-39321.6 -39320=1.6 -39322=0.4)
		-0.8, //  8:  0x999a = -26214 <- -52428 = -52428.8 / 65536.0 = -0.8 (-52428.8 -52428=0.8 -52430=1.2)
		-1.0, //  9:  0x8000 = -32768 <- -65536 = -65536.0 / 65536.0 = -1.0
		 1.2, // 10:  0x7fff =  32767 =  1.2
		-1.2  // 11:  0x8000 = -32768 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[6] = {
		0x7f, 0xff, // 0
		0x19, 0x99, // 4
		0x99, 0x9a  // 8
	};
	
	const tubyte c_expectIntegerSamplesB[6] = {
		0x66, 0x66, // 1
		0xe6, 0x66, // 5
		0x80, 0x00  // 9
	};

	const tubyte c_expectIntegerSamplesC[6] = {
		0x4c, 0xcc, // 2
		0xcc, 0xcd, // 6
		0x7f, 0xff  // 10
	};
	
	const tubyte c_expectIntegerSamplesD[6] = {
		0x33, 0x33, // 3
		0xb3, 0x33, // 7
		0x80, 0x00  // 11
	};

	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTInt16MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,6));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,6));
	ASSERT_EQ(0,memcmp(data.asioData(2,0),c_expectIntegerSamplesC,6));
	ASSERT_EQ(0,memcmp(data.asioData(3,0),c_expectIntegerSamplesD,6));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo24BitLSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7FFFFF =  8388607 <-  16777214 =  16777214.0 / 16777214.0 =  1.0
		 0.8, //  1:  0x666666 =  6710886 <-  13421772 =  13421771.2 / 16777214.0 =  0.8 ( 13421771.2  13421770=1.2  13421772=0.8)
		 0.6, //  2:  0x4CCCCC =  5033164 <-  10066328 =  10066328.4 / 16777214.0 =  0.6 ( 10066328.4  10066328=0.4  10066330=1.6)
		 0.4, //  3:  0x333333 =  3355443 <-   6710886 =   6710885.6 / 16777214.0 =  0.4 (  6710885.6   6710884=1.6   6710886=0.4)
		 0.2, //  4:  0x199999 =  1677721 <-   3355442 =   3355442.8 / 16777214.0 =  0.2 (  3355442.8   3355442=0.8   3355444=1.2)
		-0.2, //  5:  0xE66666 = -1677722 <-  -3355444 =  -3355443.2 / 16777216.0 = -0.2 ( -3355443.2  -3355442=1.2  -3355444=0.8)
		-0.4, //  6:  0xCCCCCD = -3355443 <-  -6710886 =  -6710886.4 / 16777216.0 = -0.4 ( -6710886.4  -6710886=0.4  -6710888=1.6)
		-0.6, //  7:  0xB33333 = -5033165 <- -10066330 = -10066329.6 / 16777216.0 = -0.6 (-10066329.6 -10066328=1.6 -10066330=0.4)
		-0.8, //  8:  0x99999A = -6710887 <- -13421772 = -13421772.8 / 16777216.0 = -0.8 (-13421772.8 -13421772=0.8 -13421774=1.2)
		-1.0, //  9:  0x800000 = -8388608 <- -16777216 = -16777216.0 / 16777216.0 = -1.0
		 1.2, // 10:  0x7FFFFF =  8388607 =  1.2
		-1.2  // 11:  0x800000 = -8388608 = -1.2
	};
#endif

	const tubyte c_expectIntegerSamples[36] = {
		0xff, 0xff, 0x7f, // 0
		0x66, 0x66, 0x66, // 1
		0xcc, 0xcc, 0x4c, // 2
		0x33, 0x33, 0x33, // 3
		0x99, 0x99, 0x19, // 4
		0x66, 0x66, 0xe6, // 5
		0xcd, 0xcc, 0xcc, // 6
		0x33, 0x33, 0xb3, // 7
		0x9a, 0x99, 0x99, // 8
		0x00, 0x00, 0x80, // 9
		0xff, 0xff, 0x7f, // 10
		0x00, 0x00, 0x80  // 11
	};

	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTInt24LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamples,36));
}

TEST(ASIOData,sampleTo24BitLSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7FFFFF =  8388607 <-  16777214 =  16777214.0 / 16777214.0 =  1.0
		 0.8, //  1:  0x666666 =  6710886 <-  13421772 =  13421771.2 / 16777214.0 =  0.8 ( 13421771.2  13421770=1.2  13421772=0.8)
		 0.6, //  2:  0x4CCCCC =  5033164 <-  10066328 =  10066328.4 / 16777214.0 =  0.6 ( 10066328.4  10066328=0.4  10066330=1.6)
		 0.4, //  3:  0x333333 =  3355443 <-   6710886 =   6710885.6 / 16777214.0 =  0.4 (  6710885.6   6710884=1.6   6710886=0.4)
		 0.2, //  4:  0x199999 =  1677721 <-   3355442 =   3355442.8 / 16777214.0 =  0.2 (  3355442.8   3355442=0.8   3355444=1.2)
		-0.2, //  5:  0xE66666 = -1677722 <-  -3355444 =  -3355443.2 / 16777216.0 = -0.2 ( -3355443.2  -3355442=1.2  -3355444=0.8)
		-0.4, //  6:  0xCCCCCD = -3355443 <-  -6710886 =  -6710886.4 / 16777216.0 = -0.4 ( -6710886.4  -6710886=0.4  -6710888=1.6)
		-0.6, //  7:  0xB33333 = -5033165 <- -10066330 = -10066329.6 / 16777216.0 = -0.6 (-10066329.6 -10066328=1.6 -10066330=0.4)
		-0.8, //  8:  0x99999A = -6710887 <- -13421772 = -13421772.8 / 16777216.0 = -0.8 (-13421772.8 -13421772=0.8 -13421774=1.2)
		-1.0, //  9:  0x800000 = -8388608 <- -16777216 = -16777216.0 / 16777216.0 = -1.0
		 1.2, // 10:  0x7FFFFF =  8388607 =  1.2
		-1.2  // 11:  0x800000 = -8388608 = -1.2
	};
#endif

	const tubyte c_expectIntegerSamplesA[18] = {
		0xff, 0xff, 0x7f, // 0
		0xcc, 0xcc, 0x4c, // 2
		0x99, 0x99, 0x19, // 4
		0xcd, 0xcc, 0xcc, // 6
		0x9a, 0x99, 0x99, // 8
		0xff, 0xff, 0x7f, // 10
	};

	const tubyte c_expectIntegerSamplesB[18] = {
		0x66, 0x66, 0x66, // 1
		0x33, 0x33, 0x33, // 3
		0x66, 0x66, 0xe6, // 5
		0x33, 0x33, 0xb3, // 7
		0x00, 0x00, 0x80, // 9
		0x00, 0x00, 0x80  // 11
	};
	
	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt24LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,18));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,18));
}

TEST(ASIOData,sampleTo24BitLSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7FFFFF =  8388607 <-  16777214 =  16777214.0 / 16777214.0 =  1.0
		 0.8, //  1:  0x666666 =  6710886 <-  13421772 =  13421771.2 / 16777214.0 =  0.8 ( 13421771.2  13421770=1.2  13421772=0.8)
		 0.6, //  2:  0x4CCCCC =  5033164 <-  10066328 =  10066328.4 / 16777214.0 =  0.6 ( 10066328.4  10066328=0.4  10066330=1.6)
		 0.4, //  3:  0x333333 =  3355443 <-   6710886 =   6710885.6 / 16777214.0 =  0.4 (  6710885.6   6710884=1.6   6710886=0.4)
		 0.2, //  4:  0x199999 =  1677721 <-   3355442 =   3355442.8 / 16777214.0 =  0.2 (  3355442.8   3355442=0.8   3355444=1.2)
		-0.2, //  5:  0xE66666 = -1677722 <-  -3355444 =  -3355443.2 / 16777216.0 = -0.2 ( -3355443.2  -3355442=1.2  -3355444=0.8)
		-0.4, //  6:  0xCCCCCD = -3355443 <-  -6710886 =  -6710886.4 / 16777216.0 = -0.4 ( -6710886.4  -6710886=0.4  -6710888=1.6)
		-0.6, //  7:  0xB33333 = -5033165 <- -10066330 = -10066329.6 / 16777216.0 = -0.6 (-10066329.6 -10066328=1.6 -10066330=0.4)
		-0.8, //  8:  0x99999A = -6710887 <- -13421772 = -13421772.8 / 16777216.0 = -0.8 (-13421772.8 -13421772=0.8 -13421774=1.2)
		-1.0, //  9:  0x800000 = -8388608 <- -16777216 = -16777216.0 / 16777216.0 = -1.0
		 1.2, // 10:  0x7FFFFF =  8388607 =  1.2
		-1.2  // 11:  0x800000 = -8388608 = -1.2
	};
#endif
		
	const tubyte c_expectIntegerSamplesA[9] = {
		0xff, 0xff, 0x7f, // 0
		0x99, 0x99, 0x19, // 4
		0x9a, 0x99, 0x99  // 8
	};

	const tubyte c_expectIntegerSamplesB[9] = {
		0x66, 0x66, 0x66, // 1
		0x66, 0x66, 0xe6, // 5
		0x00, 0x00, 0x80  // 9
	};
	
	const tubyte c_expectIntegerSamplesC[9] = {
		0xcc, 0xcc, 0x4c, // 2
		0xcd, 0xcc, 0xcc, // 6
		0xff, 0xff, 0x7f  // 10
	};

	const tubyte c_expectIntegerSamplesD[9] = {
		0x33, 0x33, 0x33, // 3
		0x33, 0x33, 0xb3, // 7
		0x00, 0x00, 0x80  // 11
	};
	
	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTInt24LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,9));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,9));
	ASSERT_EQ(0,memcmp(data.asioData(2,0),c_expectIntegerSamplesC,9));
	ASSERT_EQ(0,memcmp(data.asioData(3,0),c_expectIntegerSamplesD,9));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo24BitLSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamplesA[15] = {
		0x00, 0xff, 0x7f,
		0x00, 0xcc, 0x4c,
		0x00, 0x99, 0x19,
		0x00, 0xcd, 0xcc,
		0x00, 0x9a, 0x99
	};
	
	const tubyte c_expectIntegerSamplesB[15] = {
		0x00, 0x66, 0x66,
		0x00, 0x33, 0x33,
		0x00, 0x66, 0xe6,
		0x00, 0x33, 0xb3,
		0x00, 0x00, 0x80
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt24LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,15));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,15));
}

TEST(ASIOData,sampleTo24BitLSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tubyte c_expectIntegerSamplesA[15] = {
		0xff, 0xff, 0x7f,
		0xcc, 0xcc, 0x4c,
		0x99, 0x99, 0x19,
		0xcd, 0xcc, 0xcc,
		0x9a, 0x99, 0x99
	};
	
	const tubyte c_expectIntegerSamplesB[15] = {
		0x66, 0x66, 0x66,
		0x33, 0x33, 0x33,
		0x66, 0x66, 0xe6,
		0x33, 0x33, 0xb3,
		0x00, 0x00, 0x80
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt24LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,15));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,15));
}

TEST(ASIOData,sampleTo24BitLSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000
	};

	const tubyte c_expectIntegerSamplesA[15] = {
		0xff, 0xff, 0x7f,
		0xcd, 0xcc, 0x4c,
		0x9a, 0x99, 0x19,
		0xcd, 0xcc, 0xcc,
		0x9a, 0x99, 0x99
	};
	
	const tubyte c_expectIntegerSamplesB[15] = {
		0x66, 0x66, 0x66,
		0x33, 0x33, 0x33,
		0x66, 0x66, 0xe6,
		0x33, 0x33, 0xb3,
		0x00, 0x00, 0x80
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt24LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,15));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,15));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo24BitMSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7FFFFF =  8388607 <-  16777214 =  16777214.0 / 16777214.0 =  1.0
		 0.8, //  1:  0x666666 =  6710886 <-  13421772 =  13421771.2 / 16777214.0 =  0.8 ( 13421771.2  13421770=1.2  13421772=0.8)
		 0.6, //  2:  0x4CCCCC =  5033164 <-  10066328 =  10066328.4 / 16777214.0 =  0.6 ( 10066328.4  10066328=0.4  10066330=1.6)
		 0.4, //  3:  0x333333 =  3355443 <-   6710886 =   6710885.6 / 16777214.0 =  0.4 (  6710885.6   6710884=1.6   6710886=0.4)
		 0.2, //  4:  0x199999 =  1677721 <-   3355442 =   3355442.8 / 16777214.0 =  0.2 (  3355442.8   3355442=0.8   3355444=1.2)
		-0.2, //  5:  0xE66666 = -1677722 <-  -3355444 =  -3355443.2 / 16777216.0 = -0.2 ( -3355443.2  -3355442=1.2  -3355444=0.8)
		-0.4, //  6:  0xCCCCCD = -3355443 <-  -6710886 =  -6710886.4 / 16777216.0 = -0.4 ( -6710886.4  -6710886=0.4  -6710888=1.6)
		-0.6, //  7:  0xB33333 = -5033165 <- -10066330 = -10066329.6 / 16777216.0 = -0.6 (-10066329.6 -10066328=1.6 -10066330=0.4)
		-0.8, //  8:  0x99999A = -6710887 <- -13421772 = -13421772.8 / 16777216.0 = -0.8 (-13421772.8 -13421772=0.8 -13421774=1.2)
		-1.0, //  9:  0x800000 = -8388608 <- -16777216 = -16777216.0 / 16777216.0 = -1.0
		 1.2, // 10:  0x7FFFFF =  8388607 =  1.2
		-1.2  // 11:  0x800000 = -8388608 = -1.2
	};
#endif
		
	const tubyte c_expectIntegerSamples[36] = {
		0x7f, 0xff, 0xff, // 0
		0x66, 0x66, 0x66, // 1
		0x4c, 0xcc, 0xcc, // 2
		0x33, 0x33, 0x33, // 3
		0x19, 0x99, 0x99, // 4
		0xe6, 0x66, 0x66, // 5
		0xcc, 0xcc, 0xcd, // 6
		0xb3, 0x33, 0x33, // 7
		0x99, 0x99, 0x9a, // 8
		0x80, 0x00, 0x00, // 9 
		0x7f, 0xff, 0xff, // 10
		0x80, 0x00, 0x00  // 11
	};

	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTInt24MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamples,36));
}

TEST(ASIOData,sampleTo24BitMSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7FFFFF =  8388607 <-  16777214 =  16777214.0 / 16777214.0 =  1.0
		 0.8, //  1:  0x666666 =  6710886 <-  13421772 =  13421771.2 / 16777214.0 =  0.8 ( 13421771.2  13421770=1.2  13421772=0.8)
		 0.6, //  2:  0x4CCCCC =  5033164 <-  10066328 =  10066328.4 / 16777214.0 =  0.6 ( 10066328.4  10066328=0.4  10066330=1.6)
		 0.4, //  3:  0x333333 =  3355443 <-   6710886 =   6710885.6 / 16777214.0 =  0.4 (  6710885.6   6710884=1.6   6710886=0.4)
		 0.2, //  4:  0x199999 =  1677721 <-   3355442 =   3355442.8 / 16777214.0 =  0.2 (  3355442.8   3355442=0.8   3355444=1.2)
		-0.2, //  5:  0xE66666 = -1677722 <-  -3355444 =  -3355443.2 / 16777216.0 = -0.2 ( -3355443.2  -3355442=1.2  -3355444=0.8)
		-0.4, //  6:  0xCCCCCD = -3355443 <-  -6710886 =  -6710886.4 / 16777216.0 = -0.4 ( -6710886.4  -6710886=0.4  -6710888=1.6)
		-0.6, //  7:  0xB33333 = -5033165 <- -10066330 = -10066329.6 / 16777216.0 = -0.6 (-10066329.6 -10066328=1.6 -10066330=0.4)
		-0.8, //  8:  0x99999A = -6710887 <- -13421772 = -13421772.8 / 16777216.0 = -0.8 (-13421772.8 -13421772=0.8 -13421774=1.2)
		-1.0, //  9:  0x800000 = -8388608 <- -16777216 = -16777216.0 / 16777216.0 = -1.0
		 1.2, // 10:  0x7FFFFF =  8388607 =  1.2
		-1.2  // 11:  0x800000 = -8388608 = -1.2
	};
#endif

	const tubyte c_expectIntegerSamplesA[18] = {
		0x7f, 0xff, 0xff, // 0
		0x4c, 0xcc, 0xcc, // 2
		0x19, 0x99, 0x99, // 4
		0xcc, 0xcc, 0xcd, // 6
		0x99, 0x99, 0x9a, // 8
		0x7f, 0xff, 0xff  // 10
	};
	
	const tubyte c_expectIntegerSamplesB[18] = {
		0x66, 0x66, 0x66, // 1
		0x33, 0x33, 0x33, // 3
		0xe6, 0x66, 0x66, // 5
		0xb3, 0x33, 0x33, // 7
		0x80, 0x00, 0x00, // 9 
		0x80, 0x00, 0x00  // 11
	};

	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt24MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,18));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,18));
}

TEST(ASIOData,sampleTo24BitMSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7FFFFF =  8388607 <-  16777214 =  16777214.0 / 16777214.0 =  1.0
		 0.8, //  1:  0x666666 =  6710886 <-  13421772 =  13421771.2 / 16777214.0 =  0.8 ( 13421771.2  13421770=1.2  13421772=0.8)
		 0.6, //  2:  0x4CCCCC =  5033164 <-  10066328 =  10066328.4 / 16777214.0 =  0.6 ( 10066328.4  10066328=0.4  10066330=1.6)
		 0.4, //  3:  0x333333 =  3355443 <-   6710886 =   6710885.6 / 16777214.0 =  0.4 (  6710885.6   6710884=1.6   6710886=0.4)
		 0.2, //  4:  0x199999 =  1677721 <-   3355442 =   3355442.8 / 16777214.0 =  0.2 (  3355442.8   3355442=0.8   3355444=1.2)
		-0.2, //  5:  0xE66666 = -1677722 <-  -3355444 =  -3355443.2 / 16777216.0 = -0.2 ( -3355443.2  -3355442=1.2  -3355444=0.8)
		-0.4, //  6:  0xCCCCCD = -3355443 <-  -6710886 =  -6710886.4 / 16777216.0 = -0.4 ( -6710886.4  -6710886=0.4  -6710888=1.6)
		-0.6, //  7:  0xB33333 = -5033165 <- -10066330 = -10066329.6 / 16777216.0 = -0.6 (-10066329.6 -10066328=1.6 -10066330=0.4)
		-0.8, //  8:  0x99999A = -6710887 <- -13421772 = -13421772.8 / 16777216.0 = -0.8 (-13421772.8 -13421772=0.8 -13421774=1.2)
		-1.0, //  9:  0x800000 = -8388608 <- -16777216 = -16777216.0 / 16777216.0 = -1.0
		 1.2, // 10:  0x7FFFFF =  8388607 =  1.2
		-1.2  // 11:  0x800000 = -8388608 = -1.2
	};
#endif

	const tubyte c_expectIntegerSamplesA[9] = {
		0x7f, 0xff, 0xff, // 0
		0x19, 0x99, 0x99, // 4
		0x99, 0x99, 0x9a, // 8
	};
	
	const tubyte c_expectIntegerSamplesB[9] = {
		0x66, 0x66, 0x66, // 1
		0xe6, 0x66, 0x66, // 5
		0x80, 0x00, 0x00, // 9 
	};

	const tubyte c_expectIntegerSamplesC[9] = {
		0x4c, 0xcc, 0xcc, // 2
		0xcc, 0xcc, 0xcd, // 6
		0x7f, 0xff, 0xff, // 10
	};

	const tubyte c_expectIntegerSamplesD[9] = {
		0x33, 0x33, 0x33, // 3
		0xb3, 0x33, 0x33, // 7
		0x80, 0x00, 0x00  // 11
	};

	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTInt24MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,9));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,9));
	ASSERT_EQ(0,memcmp(data.asioData(2,0),c_expectIntegerSamplesC,9));
	ASSERT_EQ(0,memcmp(data.asioData(3,0),c_expectIntegerSamplesD,9));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo24BitMSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamplesA[15] = {
		0x7f, 0xff, 0x00,
		0x4c, 0xcc, 0x00,
		0x19, 0x99, 0x00,
		0xcc, 0xcd, 0x00,
		0x99, 0x9a, 0x00
	};
	
	const tubyte c_expectIntegerSamplesB[15] = {
		0x66, 0x66, 0x00,
		0x33, 0x33, 0x00,
		0xe6, 0x66, 0x00,
		0xb3, 0x33, 0x00,
		0x80, 0x00, 0x00
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt24MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,15));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,15));
}

TEST(ASIOData,sampleTo24BitMSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tubyte c_expectIntegerSamplesA[15] = {
		0x7f, 0xff, 0xff,
		0x4c, 0xcc, 0xcc,
		0x19, 0x99, 0x99,
		0xcc, 0xcc, 0xcd,
		0x99, 0x99, 0x9a
	};
	
	const tubyte c_expectIntegerSamplesB[15] = {
		0x66, 0x66, 0x66,
		0x33, 0x33, 0x33,
		0xe6, 0x66, 0x66,
		0xb3, 0x33, 0x33,
		0x80, 0x00, 0x00
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt24MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,15));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,15));
}

TEST(ASIOData,sampleTo24BitMSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000
	};

	const tubyte c_expectIntegerSamplesA[15] = {
		0x7f, 0xff, 0xff,
		0x4c, 0xcc, 0xcd,
		0x19, 0x99, 0x9a,
		0xcc, 0xcc, 0xcd,
		0x99, 0x99, 0x9a
	};
	
	const tubyte c_expectIntegerSamplesB[15] = {
		0x66, 0x66, 0x66,
		0x33, 0x33, 0x33,
		0xe6, 0x66, 0x66,
		0xb3, 0x33, 0x33,
		0x80, 0x00, 0x00
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt24MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,15));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,15));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo32BitLSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:   0x7FFFFFFF =  2147483647 <-  4294967294 =  4294967294.0 / 4294967294.0 =  1.0
		 0.8, //  1:   0x66666666 =  1717986918 <-  3435973836 =  3435973835.2 / 4294967294.0 =  0.8 ( 3435973835.2  3435973834=1.2  3435973836=0.8)
		 0.6, //  2:   0x4CCCCCCC =  1288490188 <-  2576980376 =  2576980376.4 / 4294967294.0 =  0.6 ( 2576980376.4  2576980376=0.4  2576980378=1.6)
		 0.4, //  3:   0x33333333 =   858993459 <-  1717986918 =  1717986917.6 / 4294967294.0 =  0.4 ( 1717986917.6  1717986916=1.6  1717986918=0.4)
		 0.2, //  4:   0x19999999 =   429496729 <-   858993458 =   858993458.8 / 4294967294.0 =  0.2 (  858993458.8   858993458=0.8   858993460=1.2)
		-0.2, //  5:   0xE6666666 =  -429496730 <-  -858993460 =  -858993459.2 / 4294967296.0 = -0.2 ( -858993459.2  -858993458=1.2  -858993460=0.8)
		-0.4, //  6:   0xCCCCCCCD =  -858993459 <- -1717986918 = -1717986918.4 / 4294967296.0 = -0.4 (-1717986918.4 -1717986918=0.4 -1717986920=1.6)
		-0.6, //  7:   0xB3333333 = -1288490189 <- -2576980378 = -2576980377.6 / 4294967296.0 = -0.6 (-2576980377.6 -2576980376=1.6 -2576980378=0.4)
		-0.8, //  8:   0x9999999A = -1717986918 <- -3435973836 = -3435973836.8 / 4294967296.0 = -0.8 (-3435973836.8 -3435973836=0.8 -3435973838=1.2)
		-1.0, //  9:   0x80000000 = -2147483648 <- -4294967296 = -4294967296.0 / 4294967296.0 = -1.0
		 1.2, // 10:   0x7FFFFFFF =  2147483647 =  1.2
		-1.2  // 11:   0x80000000 = -2147483648 = -1.2
	};
#endif

	const tubyte c_expectIntegerSamples[48] = {
		0xff, 0xff, 0xff, 0x7f, // 0
		0x66, 0x66, 0x66, 0x66, // 1
		0xcc, 0xcc, 0xcc, 0x4c, // 2
		0x33, 0x33, 0x33, 0x33, // 3
		0x99, 0x99, 0x99, 0x19, // 4
		0x66, 0x66, 0x66, 0xe6, // 5
		0xcd, 0xcc, 0xcc, 0xcc, // 6
		0x33, 0x33, 0x33, 0xb3, // 7
		0x9a, 0x99, 0x99, 0x99, // 8
		0x00, 0x00, 0x00, 0x80, // 9
		0xff, 0xff, 0xff, 0x7f, // 10
		0x00, 0x00, 0x00, 0x80  // 11
	};

	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTInt32LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamples,48));
}

TEST(ASIOData,sampleTo32BitLSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:   0x7FFFFFFF =  2147483647 <-  4294967294 =  4294967294.0 / 4294967294.0 =  1.0
		 0.8, //  1:   0x66666666 =  1717986918 <-  3435973836 =  3435973835.2 / 4294967294.0 =  0.8 ( 3435973835.2  3435973834=1.2  3435973836=0.8)
		 0.6, //  2:   0x4CCCCCCC =  1288490188 <-  2576980376 =  2576980376.4 / 4294967294.0 =  0.6 ( 2576980376.4  2576980376=0.4  2576980378=1.6)
		 0.4, //  3:   0x33333333 =   858993459 <-  1717986918 =  1717986917.6 / 4294967294.0 =  0.4 ( 1717986917.6  1717986916=1.6  1717986918=0.4)
		 0.2, //  4:   0x19999999 =   429496729 <-   858993458 =   858993458.8 / 4294967294.0 =  0.2 (  858993458.8   858993458=0.8   858993460=1.2)
		-0.2, //  5:   0xE6666666 =  -429496730 <-  -858993460 =  -858993459.2 / 4294967296.0 = -0.2 ( -858993459.2  -858993458=1.2  -858993460=0.8)
		-0.4, //  6:   0xCCCCCCCD =  -858993459 <- -1717986918 = -1717986918.4 / 4294967296.0 = -0.4 (-1717986918.4 -1717986918=0.4 -1717986920=1.6)
		-0.6, //  7:   0xB3333333 = -1288490189 <- -2576980378 = -2576980377.6 / 4294967296.0 = -0.6 (-2576980377.6 -2576980376=1.6 -2576980378=0.4)
		-0.8, //  8:   0x9999999A = -1717986918 <- -3435973836 = -3435973836.8 / 4294967296.0 = -0.8 (-3435973836.8 -3435973836=0.8 -3435973838=1.2)
		-1.0, //  9:   0x80000000 = -2147483648 <- -4294967296 = -4294967296.0 / 4294967296.0 = -1.0
		 1.2, // 10:   0x7FFFFFFF =  2147483647 =  1.2
		-1.2  // 11:   0x80000000 = -2147483648 = -1.2
	};
#endif

	const tubyte c_expectIntegerSamplesA[24] ={
		0xff,0xff,0xff,0x7f, // 0
		0xcc,0xcc,0xcc,0x4c, // 2
		0x99,0x99,0x99,0x19, // 4
		0xcd,0xcc,0xcc,0xcc, // 6
		0x9a,0x99,0x99,0x99, // 8
		0xff,0xff,0xff,0x7f, // 10
	};

	const tubyte c_expectIntegerSamplesB[24] ={
		0x66,0x66,0x66,0x66, // 1
		0x33,0x33,0x33,0x33, // 3
		0x66,0x66,0x66,0xe6, // 5
		0x33,0x33,0x33,0xb3, // 7
		0x00,0x00,0x00,0x80, // 9
		0x00,0x00,0x00,0x80  // 11
	};
	
	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt32LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,24));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,24));
}

TEST(ASIOData,sampleTo32BitLSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:   0x7FFFFFFF =  2147483647 <-  4294967294 =  4294967294.0 / 4294967294.0 =  1.0
		 0.8, //  1:   0x66666666 =  1717986918 <-  3435973836 =  3435973835.2 / 4294967294.0 =  0.8 ( 3435973835.2  3435973834=1.2  3435973836=0.8)
		 0.6, //  2:   0x4CCCCCCC =  1288490188 <-  2576980376 =  2576980376.4 / 4294967294.0 =  0.6 ( 2576980376.4  2576980376=0.4  2576980378=1.6)
		 0.4, //  3:   0x33333333 =   858993459 <-  1717986918 =  1717986917.6 / 4294967294.0 =  0.4 ( 1717986917.6  1717986916=1.6  1717986918=0.4)
		 0.2, //  4:   0x19999999 =   429496729 <-   858993458 =   858993458.8 / 4294967294.0 =  0.2 (  858993458.8   858993458=0.8   858993460=1.2)
		-0.2, //  5:   0xE6666666 =  -429496730 <-  -858993460 =  -858993459.2 / 4294967296.0 = -0.2 ( -858993459.2  -858993458=1.2  -858993460=0.8)
		-0.4, //  6:   0xCCCCCCCD =  -858993459 <- -1717986918 = -1717986918.4 / 4294967296.0 = -0.4 (-1717986918.4 -1717986918=0.4 -1717986920=1.6)
		-0.6, //  7:   0xB3333333 = -1288490189 <- -2576980378 = -2576980377.6 / 4294967296.0 = -0.6 (-2576980377.6 -2576980376=1.6 -2576980378=0.4)
		-0.8, //  8:   0x9999999A = -1717986918 <- -3435973836 = -3435973836.8 / 4294967296.0 = -0.8 (-3435973836.8 -3435973836=0.8 -3435973838=1.2)
		-1.0, //  9:   0x80000000 = -2147483648 <- -4294967296 = -4294967296.0 / 4294967296.0 = -1.0
		 1.2, // 10:   0x7FFFFFFF =  2147483647 =  1.2
		-1.2  // 11:   0x80000000 = -2147483648 = -1.2
	};
#endif

	const tubyte c_expectIntegerSamplesA[12] ={
		0xff,0xff,0xff,0x7f, // 0
		0x99,0x99,0x99,0x19, // 4
		0x9a,0x99,0x99,0x99  // 8
	};

	const tubyte c_expectIntegerSamplesB[12] ={
		0x66,0x66,0x66,0x66, // 1
		0x66,0x66,0x66,0xe6, // 5
		0x00,0x00,0x00,0x80  // 9
	};

	const tubyte c_expectIntegerSamplesC[12] ={
		0xcc,0xcc,0xcc,0x4c, // 2
		0xcd,0xcc,0xcc,0xcc, // 6
		0xff,0xff,0xff,0x7f  // 10
	};

	const tubyte c_expectIntegerSamplesD[12] ={
		0x33,0x33,0x33,0x33, // 3
		0x33,0x33,0x33,0xb3, // 7
		0x00,0x00,0x00,0x80  // 11
	};

	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTInt32LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,12));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,12));
	ASSERT_EQ(0,memcmp(data.asioData(2,0),c_expectIntegerSamplesC,12));
	ASSERT_EQ(0,memcmp(data.asioData(3,0),c_expectIntegerSamplesD,12));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo32BitLSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamplesA[20] = {
		0x00, 0x00, 0xff, 0x7f,
		0x00, 0x00, 0xcc, 0x4c,
		0x00, 0x00, 0x99, 0x19,
		0x00, 0x00, 0xcd, 0xcc,
		0x00, 0x00, 0x9a, 0x99
	};
	
	const tubyte c_expectIntegerSamplesB[20] = {
		0x00, 0x00, 0x66, 0x66,
		0x00, 0x00, 0x33, 0x33,
		0x00, 0x00, 0x66, 0xe6,
		0x00, 0x00, 0x33, 0xb3,
		0x00, 0x00, 0x00, 0x80
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo32BitLSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tubyte c_expectIntegerSamplesA[20] = {
		0x00, 0xff, 0xff, 0x7f,
		0x00, 0xcc, 0xcc, 0x4c,
		0x00, 0x99, 0x99, 0x19,
		0x00, 0xcd, 0xcc, 0xcc,
		0x00, 0x9a, 0x99, 0x99
	};
	
	const tubyte c_expectIntegerSamplesB[20] = {
		0x00, 0x66, 0x66, 0x66,
		0x00, 0x33, 0x33, 0x33,
		0x00, 0x66, 0x66, 0xe6,
		0x00, 0x33, 0x33, 0xb3,
		0x00, 0x00, 0x00, 0x80
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo32BitLSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000
	};

	const tubyte c_expectIntegerSamplesA[20] = {
		0xff, 0xff, 0xff, 0x7f,
		0xcc, 0xcc, 0xcc, 0x4c,
		0x99, 0x99, 0x99, 0x19,
		0xcd, 0xcc, 0xcc, 0xcc,
		0x9a, 0x99, 0x99, 0x99
	};
	
	const tubyte c_expectIntegerSamplesB[20] = {
		0x66, 0x66, 0x66, 0x66,
		0x33, 0x33, 0x33, 0x33,
		0x66, 0x66, 0x66, 0xe6,
		0x33, 0x33, 0x33, 0xb3,
		0x00, 0x00, 0x00, 0x80
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo32BitMSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:   0x7FFFFFFF =  2147483647 <-  4294967294 =  4294967294.0 / 4294967294.0 =  1.0
		 0.8, //  1:   0x66666666 =  1717986918 <-  3435973836 =  3435973835.2 / 4294967294.0 =  0.8 ( 3435973835.2  3435973834=1.2  3435973836=0.8)
		 0.6, //  2:   0x4CCCCCCC =  1288490188 <-  2576980376 =  2576980376.4 / 4294967294.0 =  0.6 ( 2576980376.4  2576980376=0.4  2576980378=1.6)
		 0.4, //  3:   0x33333333 =   858993459 <-  1717986918 =  1717986917.6 / 4294967294.0 =  0.4 ( 1717986917.6  1717986916=1.6  1717986918=0.4)
		 0.2, //  4:   0x19999999 =   429496729 <-   858993458 =   858993458.8 / 4294967294.0 =  0.2 (  858993458.8   858993458=0.8   858993460=1.2)
		-0.2, //  5:   0xE6666666 =  -429496730 <-  -858993460 =  -858993459.2 / 4294967296.0 = -0.2 ( -858993459.2  -858993458=1.2  -858993460=0.8)
		-0.4, //  6:   0xCCCCCCCD =  -858993459 <- -1717986918 = -1717986918.4 / 4294967296.0 = -0.4 (-1717986918.4 -1717986918=0.4 -1717986920=1.6)
		-0.6, //  7:   0xB3333333 = -1288490189 <- -2576980378 = -2576980377.6 / 4294967296.0 = -0.6 (-2576980377.6 -2576980376=1.6 -2576980378=0.4)
		-0.8, //  8:   0x9999999A = -1717986918 <- -3435973836 = -3435973836.8 / 4294967296.0 = -0.8 (-3435973836.8 -3435973836=0.8 -3435973838=1.2)
		-1.0, //  9:   0x80000000 = -2147483648 <- -4294967296 = -4294967296.0 / 4294967296.0 = -1.0
		 1.2, // 10:   0x7FFFFFFF =  2147483647 =  1.2
		-1.2  // 11:   0x80000000 = -2147483648 = -1.2
	};
#endif

	const tubyte c_expectIntegerSamples[48] = {
		0x7f, 0xff, 0xff, 0xff, // 0
		0x66, 0x66, 0x66, 0x66, // 1
		0x4c, 0xcc, 0xcc, 0xcc, // 2
		0x33, 0x33, 0x33, 0x33, // 3
		0x19, 0x99, 0x99, 0x99, // 4
		0xe6, 0x66, 0x66, 0x66, // 5
		0xcc, 0xcc, 0xcc, 0xcd, // 6
		0xb3, 0x33, 0x33, 0x33, // 7
		0x99, 0x99, 0x99, 0x9a, // 8
		0x80, 0x00, 0x00, 0x00, // 9
		0x7f, 0xff, 0xff, 0xff, // 10
		0x80, 0x00, 0x00, 0x00  // 11
	};

	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTInt32MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamples,48));
}

TEST(ASIOData,sampleTo32BitMSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:   0x7FFFFFFF =  2147483647 <-  4294967294 =  4294967294.0 / 4294967294.0 =  1.0
		 0.8, //  1:   0x66666666 =  1717986918 <-  3435973836 =  3435973835.2 / 4294967294.0 =  0.8 ( 3435973835.2  3435973834=1.2  3435973836=0.8)
		 0.6, //  2:   0x4CCCCCCC =  1288490188 <-  2576980376 =  2576980376.4 / 4294967294.0 =  0.6 ( 2576980376.4  2576980376=0.4  2576980378=1.6)
		 0.4, //  3:   0x33333333 =   858993459 <-  1717986918 =  1717986917.6 / 4294967294.0 =  0.4 ( 1717986917.6  1717986916=1.6  1717986918=0.4)
		 0.2, //  4:   0x19999999 =   429496729 <-   858993458 =   858993458.8 / 4294967294.0 =  0.2 (  858993458.8   858993458=0.8   858993460=1.2)
		-0.2, //  5:   0xE6666666 =  -429496730 <-  -858993460 =  -858993459.2 / 4294967296.0 = -0.2 ( -858993459.2  -858993458=1.2  -858993460=0.8)
		-0.4, //  6:   0xCCCCCCCD =  -858993459 <- -1717986918 = -1717986918.4 / 4294967296.0 = -0.4 (-1717986918.4 -1717986918=0.4 -1717986920=1.6)
		-0.6, //  7:   0xB3333333 = -1288490189 <- -2576980378 = -2576980377.6 / 4294967296.0 = -0.6 (-2576980377.6 -2576980376=1.6 -2576980378=0.4)
		-0.8, //  8:   0x9999999A = -1717986918 <- -3435973836 = -3435973836.8 / 4294967296.0 = -0.8 (-3435973836.8 -3435973836=0.8 -3435973838=1.2)
		-1.0, //  9:   0x80000000 = -2147483648 <- -4294967296 = -4294967296.0 / 4294967296.0 = -1.0
		 1.2, // 10:   0x7FFFFFFF =  2147483647 =  1.2
		-1.2  // 11:   0x80000000 = -2147483648 = -1.2
	};
#endif

	const tubyte c_expectIntegerSamplesA[24] ={
		0x7f,0xff,0xff,0xff, // 0
		0x4c,0xcc,0xcc,0xcc, // 2
		0x19,0x99,0x99,0x99, // 4
		0xcc,0xcc,0xcc,0xcd, // 6
		0x99,0x99,0x99,0x9a, // 8
		0x7f,0xff,0xff,0xff  // 10
	};

	const tubyte c_expectIntegerSamplesB[24] ={
		0x66,0x66,0x66,0x66, // 1
		0x33,0x33,0x33,0x33, // 3
		0xe6,0x66,0x66,0x66, // 5
		0xb3,0x33,0x33,0x33, // 7
		0x80,0x00,0x00,0x00, // 9
		0x80,0x00,0x00,0x00  // 11
	};

	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt32MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,24));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,24));
}

TEST(ASIOData,sampleTo32BitMSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:   0x7FFFFFFF =  2147483647 <-  4294967294 =  4294967294.0 / 4294967294.0 =  1.0
		 0.8, //  1:   0x66666666 =  1717986918 <-  3435973836 =  3435973835.2 / 4294967294.0 =  0.8 ( 3435973835.2  3435973834=1.2  3435973836=0.8)
		 0.6, //  2:   0x4CCCCCCC =  1288490188 <-  2576980376 =  2576980376.4 / 4294967294.0 =  0.6 ( 2576980376.4  2576980376=0.4  2576980378=1.6)
		 0.4, //  3:   0x33333333 =   858993459 <-  1717986918 =  1717986917.6 / 4294967294.0 =  0.4 ( 1717986917.6  1717986916=1.6  1717986918=0.4)
		 0.2, //  4:   0x19999999 =   429496729 <-   858993458 =   858993458.8 / 4294967294.0 =  0.2 (  858993458.8   858993458=0.8   858993460=1.2)
		-0.2, //  5:   0xE6666666 =  -429496730 <-  -858993460 =  -858993459.2 / 4294967296.0 = -0.2 ( -858993459.2  -858993458=1.2  -858993460=0.8)
		-0.4, //  6:   0xCCCCCCCD =  -858993459 <- -1717986918 = -1717986918.4 / 4294967296.0 = -0.4 (-1717986918.4 -1717986918=0.4 -1717986920=1.6)
		-0.6, //  7:   0xB3333333 = -1288490189 <- -2576980378 = -2576980377.6 / 4294967296.0 = -0.6 (-2576980377.6 -2576980376=1.6 -2576980378=0.4)
		-0.8, //  8:   0x9999999A = -1717986918 <- -3435973836 = -3435973836.8 / 4294967296.0 = -0.8 (-3435973836.8 -3435973836=0.8 -3435973838=1.2)
		-1.0, //  9:   0x80000000 = -2147483648 <- -4294967296 = -4294967296.0 / 4294967296.0 = -1.0
		 1.2, // 10:   0x7FFFFFFF =  2147483647 =  1.2
		-1.2  // 11:   0x80000000 = -2147483648 = -1.2
	};
#endif

	const tubyte c_expectIntegerSamplesA[12] ={
		0x7f,0xff,0xff,0xff, // 0
		0x19,0x99,0x99,0x99, // 4
		0x99,0x99,0x99,0x9a  // 8
	};

	const tubyte c_expectIntegerSamplesB[12] ={
		0x66,0x66,0x66,0x66, // 1
		0xe6,0x66,0x66,0x66, // 5
		0x80,0x00,0x00,0x00  // 9
	};

	const tubyte c_expectIntegerSamplesC[12] ={
		0x4c,0xcc,0xcc,0xcc, // 2
		0xcc,0xcc,0xcc,0xcd, // 6
		0x7f,0xff,0xff,0xff  // 10
	};

	const tubyte c_expectIntegerSamplesD[12] ={
		0x33,0x33,0x33,0x33, // 3
		0xb3,0x33,0x33,0x33, // 7
		0x80,0x00,0x00,0x00  // 11
	};

	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTInt32MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,12));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,12));
	ASSERT_EQ(0,memcmp(data.asioData(2,0),c_expectIntegerSamplesC,12));
	ASSERT_EQ(0,memcmp(data.asioData(3,0),c_expectIntegerSamplesD,12));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo32BitMSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamplesA[20] = {
		0x7f, 0xff, 0x00, 0x00,
		0x4c, 0xcc, 0x00, 0x00,
		0x19, 0x99, 0x00, 0x00,
		0xcc, 0xcd, 0x00, 0x00,
		0x99, 0x9a, 0x00, 0x00
	};
	
	const tubyte c_expectIntegerSamplesB[20] = {
		0x66, 0x66, 0x00, 0x00,
		0x33, 0x33, 0x00, 0x00,
		0xe6, 0x66, 0x00, 0x00,
		0xb3, 0x33, 0x00, 0x00,
		0x80, 0x00, 0x00, 0x00
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo32BitMSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tubyte c_expectIntegerSamplesA[20] = {
		0x7f, 0xff, 0xff, 0x00,
		0x4c, 0xcc, 0xcc, 0x00,
		0x19, 0x99, 0x99, 0x00,
		0xcc, 0xcc, 0xcd, 0x00,
		0x99, 0x99, 0x9a, 0x00
	};
	
	const tubyte c_expectIntegerSamplesB[20] = {
		0x66, 0x66, 0x66, 0x00,
		0x33, 0x33, 0x33, 0x00,
		0xe6, 0x66, 0x66, 0x00,
		0xb3, 0x33, 0x33, 0x00,
		0x80, 0x00, 0x00, 0x00
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo32BitMSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000
	};

	const tubyte c_expectIntegerSamplesA[20] = {
		0x7f, 0xff, 0xff, 0xff,
		0x4c, 0xcc, 0xcc, 0xcc,
		0x19, 0x99, 0x99, 0x99,
		0xcc, 0xcc, 0xcc, 0xcd,
		0x99, 0x99, 0x99, 0x9a
	};
	
	const tubyte c_expectIntegerSamplesB[20] = {
		0x66, 0x66, 0x66, 0x66,
		0x33, 0x33, 0x33, 0x33,
		0xe6, 0x66, 0x66, 0x66,
		0xb3, 0x33, 0x33, 0x33,
		0x80, 0x00, 0x00, 0x00
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleToFloatLSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -1.0f, -0.8f, -0.6f, -0.4f, -0.2f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0,  0.8,  0.6,  0.4,  0.2,
		-1.0, -0.8, -0.6, -0.4, -0.2,
		 1.2, -1.2
	};
#endif
	
	const tfloat32 c_expectFloatSample[12] = {
		 1.0f, // 0
		 0.8f, // 1
		 0.6f, // 2
		 0.4f, // 3
		 0.2f, // 4
		-1.0f, // 5
		-0.8f, // 6
		-0.6f, // 7
		-0.4f, // 8
		-0.2f, // 9
		 1.0f, // 10
		-1.0f  // 11
	};
	
	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTFloat32LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	tfloat32 outData[12];
	engine::writeLittleEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outData),12);
	for(int i=0;i<12;i++)
	{
		EXPECT_FLOAT_EQ(c_expectFloatSample[i],outData[i]);
	}
}

TEST(ASIOData,sampleToFloatLSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -1.0f, -0.8f, -0.6f, -0.4f, -0.2f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0,  0.8,  0.6,  0.4,  0.2,
		-1.0, -0.8, -0.6, -0.4, -0.2,
		 1.2, -1.2
	};
#endif

	const tfloat32 c_expectFloatSampleA[6] = {
		 1.0f, // 0
		 0.6f, // 2
		 0.2f, // 4
		-0.8f, // 6
		-0.4f, // 8
		 1.0f  // 10
	};

	const tfloat32 c_expectFloatSampleB[6] = {
		 0.8f, // 1
		 0.4f, // 3
		-1.0f, // 5
		-0.6f, // 7
		-0.2f, // 9
		-1.0f  // 11
	};
	
	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTFloat32LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	tfloat32 outDataA[6];
	tfloat32 outDataB[6];
	engine::writeLittleEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),6);
	engine::writeLittleEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),6);
	for(int i=0;i<6;i++)
	{
		EXPECT_FLOAT_EQ(c_expectFloatSampleA[i],outDataA[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleB[i],outDataB[i]);
	}
}

TEST(ASIOData,sampleToFloatLSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -1.0f, -0.8f, -0.6f, -0.4f, -0.2f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0,  0.8,  0.6,  0.4,  0.2,
		-1.0, -0.8, -0.6, -0.4, -0.2,
		 1.2, -1.2
	};
#endif

	const tfloat32 c_expectFloatSampleA[3] = {
		 1.0f, // 0
		 0.2f, // 4
		-0.4f  // 8
	};
	
	const tfloat32 c_expectFloatSampleB[3] = {
		 0.8f, // 1
		-1.0f, // 5
		-0.2f  // 9
	};
	
	const tfloat32 c_expectFloatSampleC[3] = {
		 0.6f, // 2
		-0.8f, // 6
		 1.0f  // 10
	};
	
	const tfloat32 c_expectFloatSampleD[3] = {
		 0.4f, // 3
		-0.6f, // 7
		-1.0f  // 11
	};
	
	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTFloat32LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	tfloat32 outDataA[3];
	tfloat32 outDataB[3];
	tfloat32 outDataC[3];
	tfloat32 outDataD[3];
	engine::writeLittleEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),3);
	engine::writeLittleEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),3);
	engine::writeLittleEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(2,0)),reinterpret_cast<tbyte *>(outDataC),3);
	engine::writeLittleEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(3,0)),reinterpret_cast<tbyte *>(outDataD),3);
	for(int i=0;i<3;i++)
	{
		EXPECT_FLOAT_EQ(c_expectFloatSampleA[i],outDataA[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleB[i],outDataB[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleC[i],outDataC[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleD[i],outDataD[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleToFloatLSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};


	const tfloat32 c_expectFloatSampleA[5] = {
		 1.0f, // 0
		 0.6f, // 2
		 0.2f, // 4
		-0.4f, // 6
		-0.8f  // 8
	};

	const tfloat32 c_expectFloatSampleB[5] = {
		 0.8f, // 1
		 0.4f, // 3
		-0.2f, // 5
		-0.6f, // 7
		-1.0f  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTFloat32LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	tfloat32 outDataA[5];
	tfloat32 outDataB[5];
	engine::writeLittleEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),5);
	engine::writeLittleEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),5);
	for(int i=0;i<5;i++)
	{
		EXPECT_NEAR(c_expectFloatSampleA[i],outDataA[i], 0.0001);
		EXPECT_NEAR(c_expectFloatSampleB[i],outDataB[i], 0.0001);
	}
}

TEST(ASIOData,sampleToFloatLSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tfloat32 c_expectFloatSampleA[5] = {
		 1.0f, // 0
		 0.6f, // 2
		 0.2f, // 4
		-0.4f, // 6
		-0.8f  // 8
	};

	const tfloat32 c_expectFloatSampleB[5] = {
		 0.8f, // 1
		 0.4f, // 3
		-0.2f, // 5
		-0.6f, // 7
		-1.0f  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTFloat32LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	tfloat32 outDataA[5];
	tfloat32 outDataB[5];
	engine::writeLittleEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),5);
	engine::writeLittleEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),5);
	for(int i=0;i<5;i++)
	{
		EXPECT_FLOAT_EQ(c_expectFloatSampleA[i],outDataA[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleB[i],outDataB[i]);
	}
}

TEST(ASIOData,sampleToFloatLSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000
	};

	const tfloat32 c_expectFloatSampleA[5] = {
		 1.0f, // 0
		 0.6f, // 2
		 0.2f, // 4
		-0.4f, // 6
		-0.8f  // 8
	};

	const tfloat32 c_expectFloatSampleB[5] = {
		 0.8f, // 1
		 0.4f, // 3
		-0.2f, // 5
		-0.6f, // 7
		-1.0f  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTFloat32LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	tfloat32 outDataA[5];
	tfloat32 outDataB[5];
	engine::writeLittleEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),5);
	engine::writeLittleEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),5);
	for(int i=0;i<5;i++)
	{
		EXPECT_FLOAT_EQ(c_expectFloatSampleA[i],outDataA[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleB[i],outDataB[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleToFloatMSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -1.0f, -0.8f, -0.6f, -0.4f, -0.2f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0,  0.8,  0.6,  0.4,  0.2,
		-1.0, -0.8, -0.6, -0.4, -0.2,
		 1.2, -1.2
	};
#endif
	
	const tfloat32 c_expectFloatSample[12] = {
		 1.0f, // 0
		 0.8f, // 1
		 0.6f, // 2
		 0.4f, // 3
		 0.2f, // 4
		-1.0f, // 5
		-0.8f, // 6
		-0.6f, // 7
		-0.4f, // 8
		-0.2f, // 9
		 1.0f, // 10
		-1.0f  // 11
	};
	
	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTFloat32MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	tfloat32 outData[12];
	engine::writeBigEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outData),12);
	for(int i=0;i<12;i++)
	{
		EXPECT_FLOAT_EQ(c_expectFloatSample[i],outData[i]);
	}
}

TEST(ASIOData,sampleToFloatMSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -1.0f, -0.8f, -0.6f, -0.4f, -0.2f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0,  0.8,  0.6,  0.4,  0.2,
		-1.0, -0.8, -0.6, -0.4, -0.2,
		 1.2, -1.2
	};
#endif

	const tfloat32 c_expectFloatSampleA[6] = {
		 1.0f, // 0
		 0.6f, // 2
		 0.2f, // 4
		-0.8f, // 6
		-0.4f, // 8
		 1.0f  // 10
	};

	const tfloat32 c_expectFloatSampleB[6] = {
		 0.8f, // 1
		 0.4f, // 3
		-1.0f, // 5
		-0.6f, // 7
		-0.2f, // 9
		-1.0f  // 11
	};
	
	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTFloat32MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	tfloat32 outDataA[6];
	tfloat32 outDataB[6];
	engine::writeBigEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),6);
	engine::writeBigEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),6);
	for(int i=0;i<6;i++)
	{
		EXPECT_FLOAT_EQ(c_expectFloatSampleA[i],outDataA[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleB[i],outDataB[i]);
	}
}

TEST(ASIOData,sampleToFloatMSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -1.0f, -0.8f, -0.6f, -0.4f, -0.2f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0,  0.8,  0.6,  0.4,  0.2,
		-1.0, -0.8, -0.6, -0.4, -0.2,
		 1.2, -1.2
	};
#endif

	const tfloat32 c_expectFloatSampleA[3] = {
		 1.0f, // 0
		 0.2f, // 4
		-0.4f  // 8
	};
	
	const tfloat32 c_expectFloatSampleB[3] = {
		 0.8f, // 1
		-1.0f, // 5
		-0.2f  // 9
	};
	
	const tfloat32 c_expectFloatSampleC[3] = {
		 0.6f, // 2
		-0.8f, // 6
		 1.0f  // 10
	};
	
	const tfloat32 c_expectFloatSampleD[3] = {
		 0.4f, // 3
		-0.6f, // 7
		-1.0f  // 11
	};
	
	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTFloat32MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	tfloat32 outDataA[3];
	tfloat32 outDataB[3];
	tfloat32 outDataC[3];
	tfloat32 outDataD[3];
	engine::writeBigEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),3);
	engine::writeBigEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),3);
	engine::writeBigEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(2,0)),reinterpret_cast<tbyte *>(outDataC),3);
	engine::writeBigEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(3,0)),reinterpret_cast<tbyte *>(outDataD),3);
	for(int i=0;i<3;i++)
	{
		EXPECT_FLOAT_EQ(c_expectFloatSampleA[i],outDataA[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleB[i],outDataB[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleC[i],outDataC[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleD[i],outDataD[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleToFloatMSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};

	const tfloat32 c_expectFloatSampleA[5] = {
		 1.0f, // 0
		 0.6f, // 2
		 0.2f, // 4
		-0.4f, // 6
		-0.8f  // 8
	};

	const tfloat32 c_expectFloatSampleB[5] = {
		 0.8f, // 1
		 0.4f, // 3
		-0.2f, // 5
		-0.6f, // 7
		-1.0f  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTFloat32MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	tfloat32 outDataA[5];
	tfloat32 outDataB[5];
	engine::writeBigEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),5);
	engine::writeBigEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),5);
	for(int i=0;i<5;i++)
	{
		EXPECT_NEAR(c_expectFloatSampleA[i],outDataA[i], 0.0001);
		EXPECT_NEAR(c_expectFloatSampleB[i],outDataB[i], 0.0001);
	}
}

TEST(ASIOData,sampleToFloatMSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tfloat32 c_expectFloatSampleA[5] = {
		 1.0f, // 0
		 0.6f, // 2
		 0.2f, // 4
		-0.4f, // 6
		-0.8f  // 8
	};

	const tfloat32 c_expectFloatSampleB[5] = {
		 0.8f, // 1
		 0.4f, // 3
		-0.2f, // 5
		-0.6f, // 7
		-1.0f  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTFloat32MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	tfloat32 outDataA[5];
	tfloat32 outDataB[5];
	engine::writeBigEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),5);
	engine::writeBigEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),5);
	for(int i=0;i<5;i++)
	{
		EXPECT_FLOAT_EQ(c_expectFloatSampleA[i],outDataA[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleB[i],outDataB[i]);
	}
}

TEST(ASIOData,sampleToFloatMSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000
	};

	const tfloat32 c_expectFloatSampleA[5] = {
		 1.0f, // 0
		 0.6f, // 2
		 0.2f, // 4
		-0.4f, // 6
		-0.8f  // 8
	};

	const tfloat32 c_expectFloatSampleB[5] = {
		 0.8f, // 1
		 0.4f, // 3
		-0.2f, // 5
		-0.6f, // 7
		-1.0f  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTFloat32MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	tfloat32 outDataA[5];
	tfloat32 outDataB[5];
	engine::writeBigEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),5);
	engine::writeBigEndian32BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),5);
	for(int i=0;i<5;i++)
	{
		EXPECT_FLOAT_EQ(c_expectFloatSampleA[i],outDataA[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleB[i],outDataB[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleToDoubleLSBConvertionMono)
{
	static tfloat64 c_Tolerance = 0.0000001;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0,  0.8,  0.6,  0.4,  0.2,
		-1.0, -0.8, -0.6, -0.4, -0.2,
		 1.2, -1.2
	};
#endif
	
	const tfloat64 c_expectFloatSample[12] = {
		 1.0, // 0
		 0.8, // 1
		 0.6, // 2
		 0.4, // 3
		 0.2, // 4
		-1.0, // 5
		-0.8, // 6
		-0.6, // 7
		-0.4, // 8
		-0.2, // 9
		 1.0, // 10
		-1.0  // 11
	};
	
	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTFloat64LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	tfloat64 outData[12];
	engine::writeLittleEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outData),12);
	for(int i=0;i<12;i++)
	{
		EXPECT_NEAR(c_expectFloatSample[i],outData[i],c_Tolerance);
	}
}

TEST(ASIOData,sampleToDoubleLSBConvertionStereo)
{
	static tfloat64 c_Tolerance = 0.0000001;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0,  0.8,  0.6,  0.4,  0.2,
		-1.0, -0.8, -0.6, -0.4, -0.2,
		 1.2, -1.2
	};
#endif
	
	const tfloat64 c_expectFloatSampleA[6] = {
		 1.0, // 0
		 0.6, // 2
		 0.2, // 4
		-0.8, // 6
		-0.4, // 8
		 1.0  // 10
	};

	const tfloat64 c_expectFloatSampleB[6] = {
		 0.8, // 1
		 0.4, // 3
		-1.0, // 5
		-0.6, // 7
		-0.2, // 9
		-1.0  // 11
	};
	
	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTFloat64LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	tfloat64 outDataA[6];
	tfloat64 outDataB[6];
	engine::writeLittleEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),6);
	engine::writeLittleEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),6);
	for(int i=0;i<6;i++)
	{
		EXPECT_NEAR(c_expectFloatSampleA[i],outDataA[i],c_Tolerance);
		EXPECT_NEAR(c_expectFloatSampleB[i],outDataB[i],c_Tolerance);
	}
}

TEST(ASIOData,sampleToDoubleLSBConvertionQuadrophonic)
{
	static tfloat64 c_Tolerance = 0.0000001;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0,  0.8,  0.6,  0.4,  0.2,
		-1.0, -0.8, -0.6, -0.4, -0.2,
		 1.2, -1.2
	};
#endif
	
	const tfloat64 c_expectFloatSampleA[3] = {
		 1.0, // 0
		 0.2, // 4
		-0.4  // 8
	};
	
	const tfloat64 c_expectFloatSampleB[3] = {
		 0.8, // 1
		-1.0, // 5
		-0.2  // 9
	};
	
	const tfloat64 c_expectFloatSampleC[3] = {
		 0.6, // 2
		-0.8, // 6
		 1.0  // 10
	};
	
	const tfloat64 c_expectFloatSampleD[3] = {
		 0.4, // 3
		-0.6, // 7
		-1.0  // 11
	};
	
	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTFloat64LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	tfloat64 outDataA[3];
	tfloat64 outDataB[3];
	tfloat64 outDataC[3];
	tfloat64 outDataD[3];
	engine::writeLittleEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),3);
	engine::writeLittleEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),3);
	engine::writeLittleEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(2,0)),reinterpret_cast<tbyte *>(outDataC),3);
	engine::writeLittleEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(3,0)),reinterpret_cast<tbyte *>(outDataD),3);
	for(int i=0;i<3;i++)
	{
		EXPECT_NEAR(c_expectFloatSampleA[i],outDataA[i],c_Tolerance);
		EXPECT_NEAR(c_expectFloatSampleB[i],outDataB[i],c_Tolerance);
		EXPECT_NEAR(c_expectFloatSampleC[i],outDataC[i],c_Tolerance);
		EXPECT_NEAR(c_expectFloatSampleD[i],outDataD[i],c_Tolerance);
	}
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleToDoubleLSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};


	const tfloat64 c_expectFloatSampleA[5] = {
		 1.0, // 0
		 0.6, // 2
		 0.2, // 4
		-0.4, // 6
		-0.8  // 8
	};

	const tfloat64 c_expectFloatSampleB[5] = {
		 0.8, // 1
		 0.4, // 3
		-0.2, // 5
		-0.6, // 7
		-1.0  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTFloat64LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	tfloat64 outDataA[5];
	tfloat64 outDataB[5];
	engine::writeLittleEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),5);
	engine::writeLittleEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),5);
	for(int i=0;i<5;i++)
	{
		EXPECT_NEAR(c_expectFloatSampleA[i],outDataA[i], 0.0001);
		EXPECT_NEAR(c_expectFloatSampleB[i],outDataB[i], 0.0001);
	}
}

TEST(ASIOData,sampleToDoubleLSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tfloat64 c_expectFloatSampleA[5] = {
		 1.0, // 0
		 0.6, // 2
		 0.2, // 4
		-0.4, // 6
		-0.8  // 8
	};

	const tfloat64 c_expectFloatSampleB[5] = {
		 0.8, // 1
		 0.4, // 3
		-0.2, // 5
		-0.6, // 7
		-1.0  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTFloat64LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	tfloat64 outDataA[5];
	tfloat64 outDataB[5];
	engine::writeLittleEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),5);
	engine::writeLittleEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),5);
	for(int i=0;i<5;i++)
	{
		EXPECT_FLOAT_EQ(c_expectFloatSampleA[i],outDataA[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleB[i],outDataB[i]);
	}
}

TEST(ASIOData,sampleToDoubleLSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000
	};

	const tfloat64 c_expectFloatSampleA[5] = {
		 1.0, // 0
		 0.6, // 2
		 0.2, // 4
		-0.4, // 6
		-0.8  // 8
	};

	const tfloat64 c_expectFloatSampleB[5] = {
		 0.8, // 1
		 0.4, // 3
		-0.2, // 5
		-0.6, // 7
		-1.0  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTFloat64LSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	tfloat64 outDataA[5];
	tfloat64 outDataB[5];
	engine::writeLittleEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),5);
	engine::writeLittleEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),5);
	for(int i=0;i<5;i++)
	{
		EXPECT_FLOAT_EQ(c_expectFloatSampleA[i],outDataA[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleB[i],outDataB[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleToDoubleMSBConvertionMono)
{
	static tfloat64 c_Tolerance = 0.0000001;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0,  0.8,  0.6,  0.4,  0.2,
		-1.0, -0.8, -0.6, -0.4, -0.2,
		 1.2, -1.2
	};
#endif
	
	const tfloat64 c_expectFloatSample[12] = {
		 1.0, // 0
		 0.8, // 1
		 0.6, // 2
		 0.4, // 3
		 0.2, // 4
		-1.0, // 5
		-0.8, // 6
		-0.6, // 7
		-0.4, // 8
		-0.2, // 9
		 1.0, // 10
		-1.0  // 11
	};
	
	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTFloat64MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	tfloat64 outData[12];
	engine::writeBigEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outData),12);
	for(int i=0;i<12;i++)
	{
		EXPECT_NEAR(c_expectFloatSample[i],outData[i],c_Tolerance);
	}
}

TEST(ASIOData,sampleToDoubleMSBConvertionStereo)
{
	static tfloat64 c_Tolerance = 0.0000001;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0,  0.8,  0.6,  0.4,  0.2,
		-1.0, -0.8, -0.6, -0.4, -0.2,
		 1.2, -1.2
	};
#endif
	
	const tfloat64 c_expectFloatSampleA[6] = {
		 1.0, // 0
		 0.6, // 2
		 0.2, // 4
		-0.8, // 6
		-0.4, // 8
		 1.0  // 10
	};

	const tfloat64 c_expectFloatSampleB[6] = {
		 0.8, // 1
		 0.4, // 3
		-1.0, // 5
		-0.6, // 7
		-0.2, // 9
		-1.0  // 11
	};
	
	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTFloat64MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	tfloat64 outDataA[6];
	tfloat64 outDataB[6];
	engine::writeBigEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),6);
	engine::writeBigEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),6);
	for(int i=0;i<6;i++)
	{
		EXPECT_NEAR(c_expectFloatSampleA[i],outDataA[i],c_Tolerance);
		EXPECT_NEAR(c_expectFloatSampleB[i],outDataB[i],c_Tolerance);
	}
}

TEST(ASIOData,sampleToDoubleMSBConvertionQuadrophonic)
{
	static tfloat64 c_Tolerance = 0.0000001;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0,  0.8,  0.6,  0.4,  0.2,
		-1.0, -0.8, -0.6, -0.4, -0.2,
		 1.2, -1.2
	};
#endif
	
	const tfloat64 c_expectFloatSampleA[3] = {
		 1.0, // 0
		 0.2, // 4
		-0.4  // 8
	};
	
	const tfloat64 c_expectFloatSampleB[3] = {
		 0.8, // 1
		-1.0, // 5
		-0.2  // 9
	};
	
	const tfloat64 c_expectFloatSampleC[3] = {
		 0.6, // 2
		-0.8, // 6
		 1.0  // 10
	};
	
	const tfloat64 c_expectFloatSampleD[3] = {
		 0.4, // 3
		-0.6, // 7
		-1.0  // 11
	};
	
	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTFloat64MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	tfloat64 outDataA[3];
	tfloat64 outDataB[3];
	tfloat64 outDataC[3];
	tfloat64 outDataD[3];
	engine::writeBigEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),3);
	engine::writeBigEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),3);
	engine::writeBigEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(2,0)),reinterpret_cast<tbyte *>(outDataC),3);
	engine::writeBigEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(3,0)),reinterpret_cast<tbyte *>(outDataD),3);
	for(int i=0;i<3;i++)
	{
		EXPECT_NEAR(c_expectFloatSampleA[i],outDataA[i],c_Tolerance);
		EXPECT_NEAR(c_expectFloatSampleB[i],outDataB[i],c_Tolerance);
		EXPECT_NEAR(c_expectFloatSampleC[i],outDataC[i],c_Tolerance);
		EXPECT_NEAR(c_expectFloatSampleD[i],outDataD[i],c_Tolerance);
	}
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleToDoubleMSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};

	const tfloat64 c_expectFloatSampleA[5] = {
		 1.0, // 0
		 0.6, // 2
		 0.2, // 4
		-0.4, // 6
		-0.8  // 8
	};

	const tfloat64 c_expectFloatSampleB[5] = {
		 0.8, // 1
		 0.4, // 3
		-0.2, // 5
		-0.6, // 7
		-1.0  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTFloat64MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	tfloat64 outDataA[5];
	tfloat64 outDataB[5];
	engine::writeBigEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),5);
	engine::writeBigEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),5);
	for(int i=0;i<5;i++)
	{
		EXPECT_NEAR(c_expectFloatSampleA[i],outDataA[i], 0.0001);
		EXPECT_NEAR(c_expectFloatSampleB[i],outDataB[i], 0.0001);
	}
}

TEST(ASIOData,sampleToDoubleMSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tfloat64 c_expectFloatSampleA[5] = {
		 1.0, // 0
		 0.6, // 2
		 0.2, // 4
		-0.4, // 6
		-0.8  // 8
	};

	const tfloat64 c_expectFloatSampleB[5] = {
		 0.8, // 1
		 0.4, // 3
		-0.2, // 5
		-0.6, // 7
		-1.0  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTFloat64MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	tfloat64 outDataA[5];
	tfloat64 outDataB[5];
	engine::writeBigEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),5);
	engine::writeBigEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),5);
	for(int i=0;i<5;i++)
	{
		EXPECT_FLOAT_EQ(c_expectFloatSampleA[i],outDataA[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleB[i],outDataB[i]);
	}
}

TEST(ASIOData,sampleToDoubleMSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000
	};

	const tfloat64 c_expectFloatSampleA[5] = {
		 1.0, // 0
		 0.6, // 2
		 0.2, // 4
		-0.4, // 6
		-0.8  // 8
	};

	const tfloat64 c_expectFloatSampleB[5] = {
		 0.8, // 1
		 0.4, // 3
		-0.2, // 5
		-0.6, // 7
		-1.0  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTFloat64MSB);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	tfloat64 outDataA[5];
	tfloat64 outDataB[5];
	engine::writeBigEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(0,0)),reinterpret_cast<tbyte *>(outDataA),5);
	engine::writeBigEndian64BitsAsNative(reinterpret_cast<const tbyte *>(data.asioData(1,0)),reinterpret_cast<tbyte *>(outDataB),5);
	for(int i=0;i<5;i++)
	{
		EXPECT_FLOAT_EQ(c_expectFloatSampleA[i],outDataA[i]);
		EXPECT_FLOAT_EQ(c_expectFloatSampleB[i],outDataB[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo16BitIn32BitLSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7fff =  32767 <-  65534 =  65534.0 / 65534.0 =  1.0
		 0.8, //  1:  0x6666 =  26214 <-  52428 =  52427.2 / 65534.0 =  0.8 ( 52427.2  52426=1.2  52428=0.8)
		 0.6, //  2:  0x4ccc =  19660 <-  39320 =  39320.4 / 65534.0 =  0.6 ( 39320.4  39320=0.4  39322=1.6)
		 0.4, //  3:  0x3333 =  13107 <-  26214 =  26213.6 / 65534.0 =  0.4 ( 26213.6  26212=1.6  26214=0.4)
		 0.2, //  4:  0x1999 =   6553 <-  13106 =  13106.8 / 65534.0 =  0.2 ( 13106.8  13106=0.8  13108=1.2)
		-0.2, //  5:  0xe666 =  -6554 <- -13108 = -13107.2 / 65536.0 = -0.2 (-13107.2 -13108=0.8 -13106=1.2)
		-0.4, //  6:  0xcccd = -13107 <- -26214 = -26214.4 / 65536.0 = -0.4 (-26214.4 -26214=0.4 -26216=1.6)
		-0.6, //  7:  0xb333 = -19661 <- -39322 = -39321.6 / 65536.0 = -0.6 (-39321.6 -39320=1.6 -39322=0.4)
		-0.8, //  8:  0x999a = -26214 <- -52428 = -52428.8 / 65536.0 = -0.8 (-52428.8 -52428=0.8 -52430=1.2)
		-1.0, //  9:  0x8000 = -32768 <- -65536 = -65536.0 / 65536.0 = -1.0
		 1.2, // 10:  0x7fff =  32767 =  1.2
		-1.2  // 11:  0x8000 = -32768 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamples[48] = {
		0xff, 0x7f, 0x00, 0x00, // 0
		0x66, 0x66, 0x00, 0x00, // 1
		0xcc, 0x4c, 0x00, 0x00, // 2
		0x33, 0x33, 0x00, 0x00, // 3
		0x99, 0x19, 0x00, 0x00, // 4
		0x66, 0xe6, 0xff, 0xff, // 5
		0xcd, 0xcc, 0xff, 0xff, // 6
		0x33, 0xb3, 0xff, 0xff, // 7
		0x9a, 0x99, 0xff, 0xff, // 8
		0x00, 0x80, 0xff, 0xff, // 9
		0xff, 0x7f, 0x00, 0x00, // 10
		0x00, 0x80, 0xff, 0xff  // 11
	};

	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTInt32LSB16);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamples,48));
}

TEST(ASIOData,sampleTo16BitIn32BitLSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7fff =  32767 <-  65534 =  65534.0 / 65534.0 =  1.0
		 0.8, //  1:  0x6666 =  26214 <-  52428 =  52427.2 / 65534.0 =  0.8 ( 52427.2  52426=1.2  52428=0.8)
		 0.6, //  2:  0x4ccc =  19660 <-  39320 =  39320.4 / 65534.0 =  0.6 ( 39320.4  39320=0.4  39322=1.6)
		 0.4, //  3:  0x3333 =  13107 <-  26214 =  26213.6 / 65534.0 =  0.4 ( 26213.6  26212=1.6  26214=0.4)
		 0.2, //  4:  0x1999 =   6553 <-  13106 =  13106.8 / 65534.0 =  0.2 ( 13106.8  13106=0.8  13108=1.2)
		-0.2, //  5:  0xe666 =  -6554 <- -13108 = -13107.2 / 65536.0 = -0.2 (-13107.2 -13108=0.8 -13106=1.2)
		-0.4, //  6:  0xcccd = -13107 <- -26214 = -26214.4 / 65536.0 = -0.4 (-26214.4 -26214=0.4 -26216=1.6)
		-0.6, //  7:  0xb333 = -19661 <- -39322 = -39321.6 / 65536.0 = -0.6 (-39321.6 -39320=1.6 -39322=0.4)
		-0.8, //  8:  0x999a = -26214 <- -52428 = -52428.8 / 65536.0 = -0.8 (-52428.8 -52428=0.8 -52430=1.2)
		-1.0, //  9:  0x8000 = -32768 <- -65536 = -65536.0 / 65536.0 = -1.0
		 1.2, // 10:  0x7fff =  32767 =  1.2
		-1.2  // 11:  0x8000 = -32768 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[24] = {
		0xff, 0x7f, 0x00, 0x00, // 0
		0xcc, 0x4c, 0x00, 0x00, // 2
		0x99, 0x19, 0x00, 0x00, // 4
		0xcd, 0xcc, 0xff, 0xff, // 6
		0x9a, 0x99, 0xff, 0xff, // 8
		0xff, 0x7f, 0x00, 0x00, // 10
	};

	const tubyte c_expectIntegerSamplesB[24] = {
		0x66, 0x66, 0x00, 0x00, // 1
		0x33, 0x33, 0x00, 0x00, // 3
		0x66, 0xe6, 0xff, 0xff, // 5
		0x33, 0xb3, 0xff, 0xff, // 7
		0x00, 0x80, 0xff, 0xff, // 9
		0x00, 0x80, 0xff, 0xff  // 11
	};

	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt32LSB16);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,24));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,24));
}

TEST(ASIOData,sampleTo16BitIn32BitLSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7fff =  32767 <-  65534 =  65534.0 / 65534.0 =  1.0
		 0.8, //  1:  0x6666 =  26214 <-  52428 =  52427.2 / 65534.0 =  0.8 ( 52427.2  52426=1.2  52428=0.8)
		 0.6, //  2:  0x4ccc =  19660 <-  39320 =  39320.4 / 65534.0 =  0.6 ( 39320.4  39320=0.4  39322=1.6)
		 0.4, //  3:  0x3333 =  13107 <-  26214 =  26213.6 / 65534.0 =  0.4 ( 26213.6  26212=1.6  26214=0.4)
		 0.2, //  4:  0x1999 =   6553 <-  13106 =  13106.8 / 65534.0 =  0.2 ( 13106.8  13106=0.8  13108=1.2)
		-0.2, //  5:  0xe666 =  -6554 <- -13108 = -13107.2 / 65536.0 = -0.2 (-13107.2 -13108=0.8 -13106=1.2)
		-0.4, //  6:  0xcccd = -13107 <- -26214 = -26214.4 / 65536.0 = -0.4 (-26214.4 -26214=0.4 -26216=1.6)
		-0.6, //  7:  0xb333 = -19661 <- -39322 = -39321.6 / 65536.0 = -0.6 (-39321.6 -39320=1.6 -39322=0.4)
		-0.8, //  8:  0x999a = -26214 <- -52428 = -52428.8 / 65536.0 = -0.8 (-52428.8 -52428=0.8 -52430=1.2)
		-1.0, //  9:  0x8000 = -32768 <- -65536 = -65536.0 / 65536.0 = -1.0
		 1.2, // 10:  0x7fff =  32767 =  1.2
		-1.2  // 11:  0x8000 = -32768 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[12] = {
		0xff, 0x7f, 0x00, 0x00, // 0
		0x99, 0x19, 0x00, 0x00, // 4
		0x9a, 0x99, 0xff, 0xff  // 8
	};

	const tubyte c_expectIntegerSamplesB[12] = {
		0x66, 0x66, 0x00, 0x00, // 1
		0x66, 0xe6, 0xff, 0xff, // 5
		0x00, 0x80, 0xff, 0xff  // 9
	};
	
	const tubyte c_expectIntegerSamplesC[12] = {
		0xcc, 0x4c, 0x00, 0x00, // 2
		0xcd, 0xcc, 0xff, 0xff, // 6
		0xff, 0x7f, 0x00, 0x00  // 10
	};
	
	const tubyte c_expectIntegerSamplesD[12] = {
		0x33, 0x33, 0x00, 0x00, // 3
		0x33, 0xb3, 0xff, 0xff, // 7
		0x00, 0x80, 0xff, 0xff  // 11
	};

	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTInt32LSB16);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,12));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,12));
	ASSERT_EQ(0,memcmp(data.asioData(2,0),c_expectIntegerSamplesC,12));
	ASSERT_EQ(0,memcmp(data.asioData(3,0),c_expectIntegerSamplesD,12));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo16BitIn32BitLSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamplesA[20] = {
		0xff, 0x7f, 0x00, 0x00, // 0
		0xcc, 0x4c, 0x00, 0x00, // 2
		0x99, 0x19, 0x00, 0x00, // 4
		0xcd, 0xcc, 0xff, 0xff, // 6
		0x9a, 0x99, 0xff, 0xff  // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x66, 0x66, 0x00, 0x00, // 1
		0x33, 0x33, 0x00, 0x00, // 3
		0x66, 0xe6, 0xff, 0xff, // 5
		0x33, 0xb3, 0xff, 0xff, // 7
		0x00, 0x80, 0xff, 0xff  // 9
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32LSB16);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo16BitIn32BitLSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[14] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000,
		0x00000080, 0x0000007f, 0x00000000, 0xffffff80
	};

	const tubyte c_expectIntegerSamplesA[28] = {
		0xff, 0x7f, 0x00, 0x00, // 0
		0xcd, 0x4c, 0x00, 0x00, // 2
		0x9a, 0x19, 0x00, 0x00, // 4
		0xcd, 0xcc, 0xff, 0xff, // 6
		0x9a, 0x99, 0xff, 0xff, // 8
		0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};

	const tubyte c_expectIntegerSamplesB[28] = {
		0x66, 0x66, 0x00, 0x00, // 1
		0x33, 0x33, 0x00, 0x00, // 3
		0x66, 0xe6, 0xff, 0xff, // 5
		0x33, 0xb3, 0xff, 0xff, // 7
		0x00, 0x80, 0xff, 0xff, // 9
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};

	ASIOData data(7,2,2);
	data.setSampleType(ASIOSTInt32LSB16);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,14 * sizeof(tint32));
	part.length() = 7;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,28));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,28));
}

TEST(ASIOData,sampleTo16BitIn32BitLSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[14] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00008000, 0x00007fff, 0x00000000, 0xffff8000
	};

	const tubyte c_expectIntegerSamplesA[28] = {
		0xff, 0x7f, 0x00, 0x00, // 0
		0xcd, 0x4c, 0x00, 0x00, // 2
		0x9a, 0x19, 0x00, 0x00, // 4
		0xcd, 0xcc, 0xff, 0xff, // 6
		0x9a, 0x99, 0xff, 0xff, // 8
		0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};

	const tubyte c_expectIntegerSamplesB[28] = {
		0x66, 0x66, 0x00, 0x00, // 1
		0x33, 0x33, 0x00, 0x00, // 3
		0x66, 0xe6, 0xff, 0xff, // 5
		0x33, 0xb3, 0xff, 0xff, // 7
		0x00, 0x80, 0xff, 0xff, // 9
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};

	ASIOData data(7,2,2);
	data.setSampleType(ASIOSTInt32LSB16);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,14 * sizeof(tint32));
	part.length() = 7;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,28));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,28));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo16BitIn32BitMSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7fff =  32767 <-  65534 =  65534.0 / 65534.0 =  1.0
		 0.8, //  1:  0x6666 =  26214 <-  52428 =  52427.2 / 65534.0 =  0.8 ( 52427.2  52426=1.2  52428=0.8)
		 0.6, //  2:  0x4ccc =  19660 <-  39320 =  39320.4 / 65534.0 =  0.6 ( 39320.4  39320=0.4  39322=1.6)
		 0.4, //  3:  0x3333 =  13107 <-  26214 =  26213.6 / 65534.0 =  0.4 ( 26213.6  26212=1.6  26214=0.4)
		 0.2, //  4:  0x1999 =   6553 <-  13106 =  13106.8 / 65534.0 =  0.2 ( 13106.8  13106=0.8  13108=1.2)
		-0.2, //  5:  0xe666 =  -6554 <- -13108 = -13107.2 / 65536.0 = -0.2 (-13107.2 -13108=0.8 -13106=1.2)
		-0.4, //  6:  0xcccd = -13107 <- -26214 = -26214.4 / 65536.0 = -0.4 (-26214.4 -26214=0.4 -26216=1.6)
		-0.6, //  7:  0xb333 = -19661 <- -39322 = -39321.6 / 65536.0 = -0.6 (-39321.6 -39320=1.6 -39322=0.4)
		-0.8, //  8:  0x999a = -26214 <- -52428 = -52428.8 / 65536.0 = -0.8 (-52428.8 -52428=0.8 -52430=1.2)
		-1.0, //  9:  0x8000 = -32768 <- -65536 = -65536.0 / 65536.0 = -1.0
		 1.2, // 10:  0x7fff =  32767 =  1.2
		-1.2  // 11:  0x8000 = -32768 = -1.2
	};
#endif

	const tubyte c_expectIntegerSamples[48] = {
		0x00, 0x00, 0x7f, 0xff, 
		0x00, 0x00, 0x66, 0x66, 
		0x00, 0x00, 0x4c, 0xcc, 
		0x00, 0x00, 0x33, 0x33, 
		0x00, 0x00, 0x19, 0x99,
		0xff, 0xff, 0xe6, 0x66, 
		0xff, 0xff, 0xcc, 0xcd, 
		0xff, 0xff, 0xb3, 0x33, 
		0xff, 0xff, 0x99, 0x9a, 
		0xff, 0xff, 0x80, 0x00,
		0x00, 0x00, 0x7f, 0xff, 
		0xff, 0xff, 0x80, 0x00
	};

	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTInt32MSB16);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamples,48));
}

TEST(ASIOData,sampleTo16BitIn32BitMSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7fff =  32767 <-  65534 =  65534.0 / 65534.0 =  1.0
		 0.8, //  1:  0x6666 =  26214 <-  52428 =  52427.2 / 65534.0 =  0.8 ( 52427.2  52426=1.2  52428=0.8)
		 0.6, //  2:  0x4ccc =  19660 <-  39320 =  39320.4 / 65534.0 =  0.6 ( 39320.4  39320=0.4  39322=1.6)
		 0.4, //  3:  0x3333 =  13107 <-  26214 =  26213.6 / 65534.0 =  0.4 ( 26213.6  26212=1.6  26214=0.4)
		 0.2, //  4:  0x1999 =   6553 <-  13106 =  13106.8 / 65534.0 =  0.2 ( 13106.8  13106=0.8  13108=1.2)
		-0.2, //  5:  0xe666 =  -6554 <- -13108 = -13107.2 / 65536.0 = -0.2 (-13107.2 -13108=0.8 -13106=1.2)
		-0.4, //  6:  0xcccd = -13107 <- -26214 = -26214.4 / 65536.0 = -0.4 (-26214.4 -26214=0.4 -26216=1.6)
		-0.6, //  7:  0xb333 = -19661 <- -39322 = -39321.6 / 65536.0 = -0.6 (-39321.6 -39320=1.6 -39322=0.4)
		-0.8, //  8:  0x999a = -26214 <- -52428 = -52428.8 / 65536.0 = -0.8 (-52428.8 -52428=0.8 -52430=1.2)
		-1.0, //  9:  0x8000 = -32768 <- -65536 = -65536.0 / 65536.0 = -1.0
		 1.2, // 10:  0x7fff =  32767 =  1.2
		-1.2  // 11:  0x8000 = -32768 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[24] = {
		0x00, 0x00, 0x7f, 0xff, // 0
		0x00, 0x00, 0x4c, 0xcc, // 2
		0x00, 0x00, 0x19, 0x99, // 4
		0xff, 0xff, 0xcc, 0xcd, // 6
		0xff, 0xff, 0x99, 0x9a, // 8
		0x00, 0x00, 0x7f, 0xff  // 10
	};
	
	const tubyte c_expectIntegerSamplesB[24] = {
		0x00, 0x00, 0x66, 0x66, // 1
		0x00, 0x00, 0x33, 0x33, // 3
		0xff, 0xff, 0xe6, 0x66, // 5
		0xff, 0xff, 0xb3, 0x33, // 7
		0xff, 0xff, 0x80, 0x00, // 9
		0xff, 0xff, 0x80, 0x00  // 11
	};

	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt32MSB16);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,24));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,24));
}

TEST(ASIOData,sampleTo16BitIn32BitMSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7fff =  32767 <-  65534 =  65534.0 / 65534.0 =  1.0
		 0.8, //  1:  0x6666 =  26214 <-  52428 =  52427.2 / 65534.0 =  0.8 ( 52427.2  52426=1.2  52428=0.8)
		 0.6, //  2:  0x4ccc =  19660 <-  39320 =  39320.4 / 65534.0 =  0.6 ( 39320.4  39320=0.4  39322=1.6)
		 0.4, //  3:  0x3333 =  13107 <-  26214 =  26213.6 / 65534.0 =  0.4 ( 26213.6  26212=1.6  26214=0.4)
		 0.2, //  4:  0x1999 =   6553 <-  13106 =  13106.8 / 65534.0 =  0.2 ( 13106.8  13106=0.8  13108=1.2)
		-0.2, //  5:  0xe666 =  -6554 <- -13108 = -13107.2 / 65536.0 = -0.2 (-13107.2 -13108=0.8 -13106=1.2)
		-0.4, //  6:  0xcccd = -13107 <- -26214 = -26214.4 / 65536.0 = -0.4 (-26214.4 -26214=0.4 -26216=1.6)
		-0.6, //  7:  0xb333 = -19661 <- -39322 = -39321.6 / 65536.0 = -0.6 (-39321.6 -39320=1.6 -39322=0.4)
		-0.8, //  8:  0x999a = -26214 <- -52428 = -52428.8 / 65536.0 = -0.8 (-52428.8 -52428=0.8 -52430=1.2)
		-1.0, //  9:  0x8000 = -32768 <- -65536 = -65536.0 / 65536.0 = -1.0
		 1.2, // 10:  0x7fff =  32767 =  1.2
		-1.2  // 11:  0x8000 = -32768 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[12] = {
		0x00, 0x00, 0x7f, 0xff, // 0
		0x00, 0x00, 0x19, 0x99, // 4
		0xff, 0xff, 0x99, 0x9a  // 8
	};

	const tubyte c_expectIntegerSamplesB[12] = {
		0x00, 0x00, 0x66, 0x66, // 1
		0xff, 0xff, 0xe6, 0x66, // 5
		0xff, 0xff, 0x80, 0x00  // 9
	};

	const tubyte c_expectIntegerSamplesC[12] = {
		0x00, 0x00, 0x4c, 0xcc, // 2
		0xff, 0xff, 0xcc, 0xcd, // 6
		0x00, 0x00, 0x7f, 0xff  // 10
	};

	const tubyte c_expectIntegerSamplesD[12] = {
		0x00, 0x00, 0x33, 0x33, // 3
		0xff, 0xff, 0xb3, 0x33, // 7
		0xff, 0xff, 0x80, 0x00  // 11
	};


	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTInt32MSB16);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,12));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,12));
	ASSERT_EQ(0,memcmp(data.asioData(2,0),c_expectIntegerSamplesC,12));
	ASSERT_EQ(0,memcmp(data.asioData(3,0),c_expectIntegerSamplesD,12));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo16BitIn32BitMSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamplesA[20] = {
		0x00, 0x00, 0x7f, 0xff, // 0
		0x00, 0x00, 0x4c, 0xcc, // 2
		0x00, 0x00, 0x19, 0x99, // 4
		0xff, 0xff, 0xcc, 0xcd, // 6
		0xff, 0xff, 0x99, 0x9a  // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x00, 0x00, 0x66, 0x66, // 1
		0x00, 0x00, 0x33, 0x33, // 3
		0xff, 0xff, 0xe6, 0x66, // 5
		0xff, 0xff, 0xb3, 0x33, // 7
		0xff, 0xff, 0x80, 0x00  // 9
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB16);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo16BitIn32BitMSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[14] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000,
		0x00000080, 0x0000007f, 0x00000000, 0xffffff80
	};

	const tubyte c_expectIntegerSamplesA[28] = {
		0x00, 0x00, 0x7f, 0xff, // 0
		0x00, 0x00, 0x4c, 0xcd, // 2
		0x00, 0x00, 0x19, 0x9a, // 4
		0xff, 0xff, 0xcc, 0xcd, // 6
		0xff, 0xff, 0x99, 0x9a, // 8
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00
	};

	const tubyte c_expectIntegerSamplesB[28] = {
		0x00, 0x00, 0x66, 0x66, // 1
		0x00, 0x00, 0x33, 0x33, // 3
		0xff, 0xff, 0xe6, 0x66, // 5
		0xff, 0xff, 0xb3, 0x33, // 7
		0xff, 0xff, 0x80, 0x00, // 9
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};

	ASIOData data(7,2,2);
	data.setSampleType(ASIOSTInt32MSB16);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,14 * sizeof(tint32));
	part.length() = 7;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,28));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,28));
}

TEST(ASIOData,sampleTo16BitIn32BitMSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[14] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00008000, 0x00007fff, 0x00000000, 0xffff8000
	};

	const tubyte c_expectIntegerSamplesA[28] = {
		0x00, 0x00, 0x7f, 0xff, // 0
		0x00, 0x00, 0x4c, 0xcd, // 2
		0x00, 0x00, 0x19, 0x9a, // 4
		0xff, 0xff, 0xcc, 0xcd, // 6
		0xff, 0xff, 0x99, 0x9a, // 8
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00
	};

	const tubyte c_expectIntegerSamplesB[28] = {
		0x00, 0x00, 0x66, 0x66, // 1
		0x00, 0x00, 0x33, 0x33, // 3
		0xff, 0xff, 0xe6, 0x66, // 5
		0xff, 0xff, 0xb3, 0x33, // 7
		0xff, 0xff, 0x80, 0x00, // 9
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};

	ASIOData data(7,2,2);
	data.setSampleType(ASIOSTInt32MSB16);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,14 * sizeof(tint32));
	part.length() = 7;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,28));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,28));
}


//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo18BitIn32BitLSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0: 0x0003FFFF =  262143 =  524286 =  524286.0 / 524286.0 =  1.0
		 0.8, //  1: 0x00033332 =  209714 =  419428 =  419428.8 / 524286.0 =  0.8 (  419428.8  419428=0.8  419430=1.2 )
		 0.6, //  2: 0x00026666 =  157286 =  314572 =  314571.6 / 524286.0 =  0.6 (  314571.6  314570=1.6  314572=0.4 )
		 0.4, //  3: 0x00019999 =  104857 =  209714 =  209714.4 / 524286.0 =  0.4 (  209714.4  209714=0.4  209716=1.6 )
		 0.2, //  4: 0x0000CCCD =   52429 =  104858 =  104857.2 / 524286.0 =  0.2 (  104857.2  104856=1.2  104858=0.8 )
		-0.2, //  5: 0xFFFF3333 =  -52429 = -104858 = -104857.6 / 524288.0 = -0.2 ( -104857.6 -104856=1.6 -104858=0.4 )
		-0.4, //  6: 0xFFFE6666 = -104858 = -209716 = -209715.2 / 524288.0 = -0.4 ( -209715.2 -209714=1.2 -209716=0.8 )
		-0.6, //  7: 0xFFFD999A = -157286 = -314572 = -314572.8 / 524288.0 = -0.6 ( -314572.8 -314572=0.8 -314574=1.2 )
		-0.8, //  8: 0xFFFCCCCD = -209715 = -419430 = -419430.4 / 524288.0 = -0.8 ( -419430.4 -419430=0.4 -419432=1.6 )
		-1.0, //  9: 0xFFFC0000 = -262144 = -524288 = -524288.0 / 524288.0 = -1.0
		 1.2, // 10: 0x0003FFFF =  1.2
		-1.2  // 11: 0xFFFC0000 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamples[48] = {
		0xff, 0xff, 0x01, 0x00, // 0
		0x99, 0x99, 0x01, 0x00, // 1
		0x33, 0x33, 0x01, 0x00, // 2
		0xcc, 0xcc, 0x00, 0x00, // 3
		0x66, 0x66, 0x00, 0x00, // 4
		0x9a, 0x99, 0xff, 0xff, // 5
		0x33, 0x33, 0xff, 0xff, // 6
		0xcd, 0xcc, 0xfe, 0xff, // 7
		0x66, 0x66, 0xfe, 0xff, // 8
		0x00, 0x00, 0xfe, 0xff, // 9
		0xff, 0xff, 0x01, 0x00, // 10
		0x00, 0x00, 0xfe, 0xff  // 11
	};

	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTInt32LSB18);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamples,48));
}

TEST(ASIOData,sampleTo18BitIn32BitLSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0: 0x0003FFFF =  262143 =  524286 =  524286.0 / 524286.0 =  1.0
		 0.8, //  1: 0x00033332 =  209714 =  419428 =  419428.8 / 524286.0 =  0.8 (  419428.8  419428=0.8  419430=1.2 )
		 0.6, //  2: 0x00026666 =  157286 =  314572 =  314571.6 / 524286.0 =  0.6 (  314571.6  314570=1.6  314572=0.4 )
		 0.4, //  3: 0x00019999 =  104857 =  209714 =  209714.4 / 524286.0 =  0.4 (  209714.4  209714=0.4  209716=1.6 )
		 0.2, //  4: 0x0000CCCD =   52429 =  104858 =  104857.2 / 524286.0 =  0.2 (  104857.2  104856=1.2  104858=0.8 )
		-0.2, //  5: 0xFFFF3333 =  -52429 = -104858 = -104857.6 / 524288.0 = -0.2 ( -104857.6 -104856=1.6 -104858=0.4 )
		-0.4, //  6: 0xFFFE6666 = -104858 = -209716 = -209715.2 / 524288.0 = -0.4 ( -209715.2 -209714=1.2 -209716=0.8 )
		-0.6, //  7: 0xFFFD999A = -157286 = -314572 = -314572.8 / 524288.0 = -0.6 ( -314572.8 -314572=0.8 -314574=1.2 )
		-0.8, //  8: 0xFFFCCCCD = -209715 = -419430 = -419430.4 / 524288.0 = -0.8 ( -419430.4 -419430=0.4 -419432=1.6 )
		-1.0, //  9: 0xFFFC0000 = -262144 = -524288 = -524288.0 / 524288.0 = -1.0
		 1.2, // 10: 0x0003FFFF =  1.2
		-1.2  // 11: 0xFFFC0000 = -1.2
	};
#endif

	const tubyte c_expectIntegerSamplesA[24] ={
		0xff,0xff,0x01,0x00, // 0
		0x33,0x33,0x01,0x00, // 2
		0x66,0x66,0x00,0x00, // 4
		0x33,0x33,0xff,0xff, // 6
		0x66,0x66,0xfe,0xff, // 8
		0xff,0xff,0x01,0x00, // 10
	};

	const tubyte c_expectIntegerSamplesB[24] ={
		0x99,0x99,0x01,0x00, // 1
		0xcc,0xcc,0x00,0x00, // 3
		0x9a,0x99,0xff,0xff, // 5
		0xcd,0xcc,0xfe,0xff, // 7
		0x00,0x00,0xfe,0xff, // 9
		0x00,0x00,0xfe,0xff  // 11
	};
	
	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt32LSB18);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,24));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,24));
}

TEST(ASIOData,sampleTo18BitIn32BitLSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0: 0x0003FFFF =  262143 =  524286 =  524286.0 / 524286.0 =  1.0
		 0.8, //  1: 0x00033332 =  209714 =  419428 =  419428.8 / 524286.0 =  0.8 (  419428.8  419428=0.8  419430=1.2 )
		 0.6, //  2: 0x00026666 =  157286 =  314572 =  314571.6 / 524286.0 =  0.6 (  314571.6  314570=1.6  314572=0.4 )
		 0.4, //  3: 0x00019999 =  104857 =  209714 =  209714.4 / 524286.0 =  0.4 (  209714.4  209714=0.4  209716=1.6 )
		 0.2, //  4: 0x0000CCCD =   52429 =  104858 =  104857.2 / 524286.0 =  0.2 (  104857.2  104856=1.2  104858=0.8 )
		-0.2, //  5: 0xFFFF3333 =  -52429 = -104858 = -104857.6 / 524288.0 = -0.2 ( -104857.6 -104856=1.6 -104858=0.4 )
		-0.4, //  6: 0xFFFE6666 = -104858 = -209716 = -209715.2 / 524288.0 = -0.4 ( -209715.2 -209714=1.2 -209716=0.8 )
		-0.6, //  7: 0xFFFD999A = -157286 = -314572 = -314572.8 / 524288.0 = -0.6 ( -314572.8 -314572=0.8 -314574=1.2 )
		-0.8, //  8: 0xFFFCCCCD = -209715 = -419430 = -419430.4 / 524288.0 = -0.8 ( -419430.4 -419430=0.4 -419432=1.6 )
		-1.0, //  9: 0xFFFC0000 = -262144 = -524288 = -524288.0 / 524288.0 = -1.0
		 1.2, // 10: 0x0003FFFF =  1.2
		-1.2  // 11: 0xFFFC0000 = -1.2
	};
#endif

	const tubyte c_expectIntegerSamplesA[12] ={
		0xff,0xff,0x01,0x00, // 0
		0x66,0x66,0x00,0x00, // 4
		0x66,0x66,0xfe,0xff  // 8
	};

	const tubyte c_expectIntegerSamplesB[12] ={
		0x99,0x99,0x01,0x00, // 1
		0x9a,0x99,0xff,0xff, // 5
		0x00,0x00,0xfe,0xff  // 9
	};

	const tubyte c_expectIntegerSamplesC[12] ={
		0x33,0x33,0x01,0x00, // 2
		0x33,0x33,0xff,0xff, // 6
		0xff,0xff,0x01,0x00  // 10
	};

	const tubyte c_expectIntegerSamplesD[12] ={
		0xcc,0xcc,0x00,0x00, // 3
		0xcd,0xcc,0xfe,0xff, // 7
		0x00,0x00,0xfe,0xff  // 11
	};

	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTInt32LSB18);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,12));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,12));
	ASSERT_EQ(0,memcmp(data.asioData(2,0),c_expectIntegerSamplesC,12));
	ASSERT_EQ(0,memcmp(data.asioData(3,0),c_expectIntegerSamplesD,12));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo18BitIn32BitLSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamplesA[20] = {
		0xfc, 0xff, 0x01, 0x00, // 0
		0x30, 0x33, 0x01, 0x00, // 2
		0x64, 0x66, 0x00, 0x00, // 4
		0x34, 0x33, 0xff, 0xff, // 6
		0x68, 0x66, 0xfe, 0xff  // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x98, 0x99, 0x01, 0x00, // 1
		0xcc, 0xcc, 0x00, 0x00, // 3
		0x98, 0x99, 0xff, 0xff, // 5
		0xcc, 0xcc, 0xfe, 0xff, // 7
		0x00, 0x00, 0xfe, 0xff  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32LSB18);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo18BitIn32BitLSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tubyte c_expectIntegerSamplesA[20] = {
		0xff, 0xff, 0x01, 0x00, // 0
		0x33, 0x33, 0x01, 0x00, // 2
		0x66, 0x66, 0x00, 0x00, // 4
		0x33, 0x33, 0xff, 0xff, // 6
		0x66, 0x66, 0xfe, 0xff  // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x9a, 0x99, 0x01, 0x00, // 1
		0xcd, 0xcc, 0x00, 0x00, // 3
		0x9a, 0x99, 0xff, 0xff, // 5
		0xcd, 0xcc, 0xfe, 0xff, // 7
		0x00, 0x00, 0xfe, 0xff  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32LSB18);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo18BitIn32BitLSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000
	};
	
	const tubyte c_expectIntegerSamplesA[20] = {
		0xff, 0xff, 0x01, 0x00, // 0
		0x33, 0x33, 0x01, 0x00, // 2
		0x66, 0x66, 0x00, 0x00, // 4
		0x33, 0x33, 0xff, 0xff, // 6
		0x66, 0x66, 0xfe, 0xff  // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x9a, 0x99, 0x01, 0x00, // 1
		0xcd, 0xcc, 0x00, 0x00, // 3
		0x9a, 0x99, 0xff, 0xff, // 5
		0xcd, 0xcc, 0xfe, 0xff, // 7
		0x00, 0x00, 0xfe, 0xff  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32LSB18);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo18BitIn32BitMSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0: 0x0003FFFF =  262143 =  524286 =  524286.0 / 524286.0 =  1.0
		 0.8, //  1: 0x00033332 =  209714 =  419428 =  419428.8 / 524286.0 =  0.8 (  419428.8  419428=0.8  419430=1.2 )
		 0.6, //  2: 0x00026666 =  157286 =  314572 =  314571.6 / 524286.0 =  0.6 (  314571.6  314570=1.6  314572=0.4 )
		 0.4, //  3: 0x00019999 =  104857 =  209714 =  209714.4 / 524286.0 =  0.4 (  209714.4  209714=0.4  209716=1.6 )
		 0.2, //  4: 0x0000CCCD =   52429 =  104858 =  104857.2 / 524286.0 =  0.2 (  104857.2  104856=1.2  104858=0.8 )
		-0.2, //  5: 0xFFFF3333 =  -52429 = -104858 = -104857.6 / 524288.0 = -0.2 ( -104857.6 -104856=1.6 -104858=0.4 )
		-0.4, //  6: 0xFFFE6666 = -104858 = -209716 = -209715.2 / 524288.0 = -0.4 ( -209715.2 -209714=1.2 -209716=0.8 )
		-0.6, //  7: 0xFFFD999A = -157286 = -314572 = -314572.8 / 524288.0 = -0.6 ( -314572.8 -314572=0.8 -314574=1.2 )
		-0.8, //  8: 0xFFFCCCCD = -209715 = -419430 = -419430.4 / 524288.0 = -0.8 ( -419430.4 -419430=0.4 -419432=1.6 )
		-1.0, //  9: 0xFFFC0000 = -262144 = -524288 = -524288.0 / 524288.0 = -1.0
		 1.2, // 10: 0x0003FFFF =  1.2
		-1.2  // 11: 0xFFFC0000 = -1.2
	};
#endif

	const tubyte c_expectIntegerSamples[48] = {
		0x00, 0x01, 0xff, 0xff, // 0
		0x00, 0x01, 0x99, 0x99, // 1
		0x00, 0x01, 0x33, 0x33, // 2
		0x00, 0x00, 0xcc, 0xcc, // 3
		0x00, 0x00, 0x66, 0x66, // 4
		0xff, 0xff, 0x99, 0x9a, // 5
		0xff, 0xff, 0x33, 0x33, // 6
		0xff, 0xfe, 0xcc, 0xcd, // 7
		0xff, 0xfe, 0x66, 0x66, // 8
		0xff, 0xfe, 0x00, 0x00, // 9
		0x00, 0x01, 0xff, 0xff, // 10
		0xff, 0xfe, 0x00, 0x00  // 11
	};

	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTInt32MSB18);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamples,48));
}

TEST(ASIOData,sampleTo18BitIn32BitMSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0: 0x0003FFFF =  262143 =  524286 =  524286.0 / 524286.0 =  1.0
		 0.8, //  1: 0x00033332 =  209714 =  419428 =  419428.8 / 524286.0 =  0.8 (  419428.8  419428=0.8  419430=1.2 )
		 0.6, //  2: 0x00026666 =  157286 =  314572 =  314571.6 / 524286.0 =  0.6 (  314571.6  314570=1.6  314572=0.4 )
		 0.4, //  3: 0x00019999 =  104857 =  209714 =  209714.4 / 524286.0 =  0.4 (  209714.4  209714=0.4  209716=1.6 )
		 0.2, //  4: 0x0000CCCD =   52429 =  104858 =  104857.2 / 524286.0 =  0.2 (  104857.2  104856=1.2  104858=0.8 )
		-0.2, //  5: 0xFFFF3333 =  -52429 = -104858 = -104857.6 / 524288.0 = -0.2 ( -104857.6 -104856=1.6 -104858=0.4 )
		-0.4, //  6: 0xFFFE6666 = -104858 = -209716 = -209715.2 / 524288.0 = -0.4 ( -209715.2 -209714=1.2 -209716=0.8 )
		-0.6, //  7: 0xFFFD999A = -157286 = -314572 = -314572.8 / 524288.0 = -0.6 ( -314572.8 -314572=0.8 -314574=1.2 )
		-0.8, //  8: 0xFFFCCCCD = -209715 = -419430 = -419430.4 / 524288.0 = -0.8 ( -419430.4 -419430=0.4 -419432=1.6 )
		-1.0, //  9: 0xFFFC0000 = -262144 = -524288 = -524288.0 / 524288.0 = -1.0
		 1.2, // 10: 0x0003FFFF =  1.2
		-1.2  // 11: 0xFFFC0000 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[24] ={
		0x00,0x01,0xff,0xff, // 0
		0x00,0x01,0x33,0x33, // 2
		0x00,0x00,0x66,0x66, // 4
		0xff,0xff,0x33,0x33, // 6
		0xff,0xfe,0x66,0x66, // 8
		0x00,0x01,0xff,0xff  // 10
	};

	const tubyte c_expectIntegerSamplesB[24] ={
		0x00,0x01,0x99,0x99, // 1
		0x00,0x00,0xcc,0xcc, // 3
		0xff,0xff,0x99,0x9a, // 5
		0xff,0xfe,0xcc,0xcd, // 7
		0xff,0xfe,0x00,0x00, // 9
		0xff,0xfe,0x00,0x00  // 11
	};

	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt32MSB18);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,24));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,24));
}

TEST(ASIOData,sampleTo18BitIn32BitMSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0: 0x0003FFFF =  262143 =  524286 =  524286.0 / 524286.0 =  1.0
		 0.8, //  1: 0x00033332 =  209714 =  419428 =  419428.8 / 524286.0 =  0.8 (  419428.8  419428=0.8  419430=1.2 )
		 0.6, //  2: 0x00026666 =  157286 =  314572 =  314571.6 / 524286.0 =  0.6 (  314571.6  314570=1.6  314572=0.4 )
		 0.4, //  3: 0x00019999 =  104857 =  209714 =  209714.4 / 524286.0 =  0.4 (  209714.4  209714=0.4  209716=1.6 )
		 0.2, //  4: 0x0000CCCD =   52429 =  104858 =  104857.2 / 524286.0 =  0.2 (  104857.2  104856=1.2  104858=0.8 )
		-0.2, //  5: 0xFFFF3333 =  -52429 = -104858 = -104857.6 / 524288.0 = -0.2 ( -104857.6 -104856=1.6 -104858=0.4 )
		-0.4, //  6: 0xFFFE6666 = -104858 = -209716 = -209715.2 / 524288.0 = -0.4 ( -209715.2 -209714=1.2 -209716=0.8 )
		-0.6, //  7: 0xFFFD999A = -157286 = -314572 = -314572.8 / 524288.0 = -0.6 ( -314572.8 -314572=0.8 -314574=1.2 )
		-0.8, //  8: 0xFFFCCCCD = -209715 = -419430 = -419430.4 / 524288.0 = -0.8 ( -419430.4 -419430=0.4 -419432=1.6 )
		-1.0, //  9: 0xFFFC0000 = -262144 = -524288 = -524288.0 / 524288.0 = -1.0
		 1.2, // 10: 0x0003FFFF =  1.2
		-1.2  // 11: 0xFFFC0000 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[12] ={
		0x00,0x01,0xff,0xff, // 0
		0x00,0x00,0x66,0x66, // 4
		0xff,0xfe,0x66,0x66  // 8
	};

	const tubyte c_expectIntegerSamplesB[12] ={
		0x00,0x01,0x99,0x99, // 1
		0xff,0xff,0x99,0x9a, // 5
		0xff,0xfe,0x00,0x00  // 9
	};

	const tubyte c_expectIntegerSamplesC[12] ={
		0x00,0x01,0x33,0x33, // 2
		0xff,0xff,0x33,0x33, // 6
		0x00,0x01,0xff,0xff  // 10
	};

	const tubyte c_expectIntegerSamplesD[12] ={
		0x00,0x00,0xcc,0xcc, // 3
		0xff,0xfe,0xcc,0xcd, // 7
		0xff,0xfe,0x00,0x00  // 11
	};
	
	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTInt32MSB18);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,12));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,12));
	ASSERT_EQ(0,memcmp(data.asioData(2,0),c_expectIntegerSamplesC,12));
	ASSERT_EQ(0,memcmp(data.asioData(3,0),c_expectIntegerSamplesD,12));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo18BitIn32BitMSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamplesA[20] = {
		0x00, 0x01, 0xff, 0xfc, // 0
		0x00, 0x01, 0x33, 0x30, // 2
		0x00, 0x00, 0x66, 0x64, // 4
		0xff, 0xff, 0x33, 0x34, // 6
		0xff, 0xfe, 0x66, 0x68  // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x00, 0x01, 0x99, 0x98, // 1
		0x00, 0x00, 0xcc, 0xcc, // 3
		0xff, 0xff, 0x99, 0x98, // 5
		0xff, 0xfe, 0xcc, 0xcc, // 7
		0xff, 0xfe, 0x00, 0x00  // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB18);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo18BitIn32BitMSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};
		
	const tubyte c_expectIntegerSamplesA[20] = {
		0x00, 0x01, 0xff, 0xff, // 0
		0x00, 0x01, 0x33, 0x33, // 2
		0x00, 0x00, 0x66, 0x66, // 4
		0xff, 0xff, 0x33, 0x33, // 6
		0xff, 0xfe, 0x66, 0x66, // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x00, 0x01, 0x99, 0x9a, // 1
		0x00, 0x00, 0xcc, 0xcd, // 3
		0xff, 0xff, 0x99, 0x9a, // 5
		0xff, 0xfe, 0xcc, 0xcd, // 7
		0xff, 0xfe, 0x00, 0x00, // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB18);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo18BitIn32BitMSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000
	};
	
	const tubyte c_expectIntegerSamplesA[20] = {
		0x00, 0x01, 0xff, 0xff, // 0
		0x00, 0x01, 0x33, 0x33, // 2
		0x00, 0x00, 0x66, 0x66, // 4
		0xff, 0xff, 0x33, 0x33, // 6
		0xff, 0xfe, 0x66, 0x66, // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x00, 0x01, 0x99, 0x9a, // 1
		0x00, 0x00, 0xcc, 0xcd, // 3
		0xff, 0xff, 0x99, 0x9a, // 5
		0xff, 0xfe, 0xcc, 0xcd, // 7
		0xff, 0xfe, 0x00, 0x00, // 9
	};
	
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB18);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo20BitIn32BitLSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0: = 0x0007FFFF =  524287 =  1048574 =  1048574.0 / 1048574.0 =  1.0
		 0.8, //  1: = 0x00066666 =  419430 =   838860 =   838859.2 / 1048574.0 =  0.8 (  838859.2  838858=1.2  838860=0.8 )
		 0.6, //  2: = 0x0004CCCC =  314572 =   629144 =   629144.4 / 1048574.0 =  0.6 (  629144.4  629144=0.4  629146=1.6 )
		 0.4, //  3: = 0x00033333 =  209715 =   419430 =   419429.6 / 1048574.0 =  0.4 (  419429.6  419428=1.6  419430=0.4 )
		 0.2, //  4: = 0x00019999 =  104857 =   209714 =   209714.8 / 1048574.0 =  0.2 (  209714.8  209714=0.8  209716=1.2 )
		-0.2, //  5: = 0xFFFE6666 = -104858 =  -209716 =  -209715.2 / 1048576.0 = -0.2 ( -209715.2 -209714=1.2 -209716=0.8 )
		-0.4, //  6: = 0xFFFCCCCD = -209715 =  -419430 =  -419430.4 / 1048576.0 = -0.4 ( -419430.4 -419430=0.4 -419432=1.6 )
		-0.6, //  7: = 0xFFFB3333 = -314573 =  -629146 =  -629145.6 / 1048576.0 = -0.6 ( -629145.6 -629144=1.6 -629146=0.4 )
		-0.8, //  8: = 0xFFF9999A = -419430 =  -838860 =  -838860.8 / 1048576.0 = -0.8 ( -838860.8 -838860=0.8 -838862=1.2 )
		-1.0, //  9: = 0xFFF80000 = -524288 = -1048576 = -1048576.0 / 1048576.0 = -1.0
		 1.2, // 10: = 0x0007FFFF =  1.2
		-1.2  // 11: = 0xFFF80000 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamples[48] = {
		0xff, 0xff, 0x07, 0x00, // 0
		0x66, 0x66, 0x06, 0x00, // 1
		0xcc, 0xcc, 0x04, 0x00, // 2
		0x33, 0x33, 0x03, 0x00, // 3
		0x99, 0x99, 0x01, 0x00, // 4
		0x66, 0x66, 0xfe, 0xff, // 5
		0xcd, 0xcc, 0xfc, 0xff, // 6
		0x33, 0x33, 0xfb, 0xff, // 7
		0x9a, 0x99, 0xf9, 0xff, // 8
		0x00, 0x00, 0xf8, 0xff, // 9
		0xff, 0xff, 0x07, 0x00, // 10
		0x00, 0x00, 0xf8, 0xff  // 11
	};

	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTInt32LSB20);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamples,48));
}

TEST(ASIOData,sampleTo20BitIn32BitLSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0: = 0x0007FFFF =  524287 =  1048574 =  1048574.0 / 1048574.0 =  1.0
		 0.8, //  1: = 0x00066666 =  419430 =   838860 =   838859.2 / 1048574.0 =  0.8 (  838859.2  838858=1.2  838860=0.8 )
		 0.6, //  2: = 0x0004CCCC =  314572 =   629144 =   629144.4 / 1048574.0 =  0.6 (  629144.4  629144=0.4  629146=1.6 )
		 0.4, //  3: = 0x00033333 =  209715 =   419430 =   419429.6 / 1048574.0 =  0.4 (  419429.6  419428=1.6  419430=0.4 )
		 0.2, //  4: = 0x00019999 =  104857 =   209714 =   209714.8 / 1048574.0 =  0.2 (  209714.8  209714=0.8  209716=1.2 )
		-0.2, //  5: = 0xFFFE6666 = -104858 =  -209716 =  -209715.2 / 1048576.0 = -0.2 ( -209715.2 -209714=1.2 -209716=0.8 )
		-0.4, //  6: = 0xFFFCCCCD = -209715 =  -419430 =  -419430.4 / 1048576.0 = -0.4 ( -419430.4 -419430=0.4 -419432=1.6 )
		-0.6, //  7: = 0xFFFB3333 = -314573 =  -629146 =  -629145.6 / 1048576.0 = -0.6 ( -629145.6 -629144=1.6 -629146=0.4 )
		-0.8, //  8: = 0xFFF9999A = -419430 =  -838860 =  -838860.8 / 1048576.0 = -0.8 ( -838860.8 -838860=0.8 -838862=1.2 )
		-1.0, //  9: = 0xFFF80000 = -524288 = -1048576 = -1048576.0 / 1048576.0 = -1.0
		 1.2, // 10: = 0x0007FFFF =  1.2
		-1.2  // 11: = 0xFFF80000 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[24] = {
		0xff, 0xff, 0x07, 0x00, // 0
		0xcc, 0xcc, 0x04, 0x00, // 2
		0x99, 0x99, 0x01, 0x00, // 4
		0xcd, 0xcc, 0xfc, 0xff, // 6
		0x9a, 0x99, 0xf9, 0xff, // 8
		0xff, 0xff, 0x07, 0x00  // 10
	};

	const tubyte c_expectIntegerSamplesB[24] = {
		0x66, 0x66, 0x06, 0x00, // 1
		0x33, 0x33, 0x03, 0x00, // 3
		0x66, 0x66, 0xfe, 0xff, // 5
		0x33, 0x33, 0xfb, 0xff, // 7
		0x00, 0x00, 0xf8, 0xff, // 9
		0x00, 0x00, 0xf8, 0xff  // 11
	};

	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt32LSB20);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,24));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,24));
}

TEST(ASIOData,sampleTo20BitIn32BitLSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0: = 0x0007FFFF =  524287 =  1048574 =  1048574.0 / 1048574.0 =  1.0
		 0.8, //  1: = 0x00066666 =  419430 =   838860 =   838859.2 / 1048574.0 =  0.8 (  838859.2  838858=1.2  838860=0.8 )
		 0.6, //  2: = 0x0004CCCC =  314572 =   629144 =   629144.4 / 1048574.0 =  0.6 (  629144.4  629144=0.4  629146=1.6 )
		 0.4, //  3: = 0x00033333 =  209715 =   419430 =   419429.6 / 1048574.0 =  0.4 (  419429.6  419428=1.6  419430=0.4 )
		 0.2, //  4: = 0x00019999 =  104857 =   209714 =   209714.8 / 1048574.0 =  0.2 (  209714.8  209714=0.8  209716=1.2 )
		-0.2, //  5: = 0xFFFE6666 = -104858 =  -209716 =  -209715.2 / 1048576.0 = -0.2 ( -209715.2 -209714=1.2 -209716=0.8 )
		-0.4, //  6: = 0xFFFCCCCD = -209715 =  -419430 =  -419430.4 / 1048576.0 = -0.4 ( -419430.4 -419430=0.4 -419432=1.6 )
		-0.6, //  7: = 0xFFFB3333 = -314573 =  -629146 =  -629145.6 / 1048576.0 = -0.6 ( -629145.6 -629144=1.6 -629146=0.4 )
		-0.8, //  8: = 0xFFF9999A = -419430 =  -838860 =  -838860.8 / 1048576.0 = -0.8 ( -838860.8 -838860=0.8 -838862=1.2 )
		-1.0, //  9: = 0xFFF80000 = -524288 = -1048576 = -1048576.0 / 1048576.0 = -1.0
		 1.2, // 10: = 0x0007FFFF =  1.2
		-1.2  // 11: = 0xFFF80000 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[12] = {
		0xff, 0xff, 0x07, 0x00, // 0
		0x99, 0x99, 0x01, 0x00, // 4
		0x9a, 0x99, 0xf9, 0xff  // 8
	};

	const tubyte c_expectIntegerSamplesB[12] = {
		0x66, 0x66, 0x06, 0x00, // 1
		0x66, 0x66, 0xfe, 0xff, // 5
		0x00, 0x00, 0xf8, 0xff  // 9
	};
	
	const tubyte c_expectIntegerSamplesC[12] = {
		0xcc, 0xcc, 0x04, 0x00, // 2
		0xcd, 0xcc, 0xfc, 0xff, // 6
		0xff, 0xff, 0x07, 0x00  // 10
	};
	
	const tubyte c_expectIntegerSamplesD[12] = {
		0x33, 0x33, 0x03, 0x00, // 3
		0x33, 0x33, 0xfb, 0xff, // 7
		0x00, 0x00, 0xf8, 0xff  // 11
	};

	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTInt32LSB20);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,12));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,12));
	ASSERT_EQ(0,memcmp(data.asioData(2,0),c_expectIntegerSamplesC,12));
	ASSERT_EQ(0,memcmp(data.asioData(3,0),c_expectIntegerSamplesD,12));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo20BitIn32BitLSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamplesA[20] = {
		0xf0, 0xff, 0x07, 0x00, // 0
		0xc0, 0xcc, 0x04, 0x00, // 2
		0x90, 0x99, 0x01, 0x00, // 4
		0xd0, 0xcc, 0xfc, 0xff, // 6
		0xa0, 0x99, 0xf9, 0xff, // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x60, 0x66, 0x06, 0x00, // 1
		0x30, 0x33, 0x03, 0x00, // 3
		0x60, 0x66, 0xfe, 0xff, // 5
		0x30, 0x33, 0xfb, 0xff, // 7
		0x00, 0x00, 0xf8, 0xff  // 9
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32LSB20);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo20BitIn32BitLSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tubyte c_expectIntegerSamplesA[20] = {
		0xff, 0xff, 0x07, 0x00, // 0
		0xcd, 0xcc, 0x04, 0x00, // 2
		0x9a, 0x99, 0x01, 0x00, // 4
		0xcd, 0xcc, 0xfc, 0xff, // 6
		0x9a, 0x99, 0xf9, 0xff, // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x66, 0x66, 0x06, 0x00, // 1
		0x33, 0x33, 0x03, 0x00, // 3
		0x66, 0x66, 0xfe, 0xff, // 5
		0x33, 0x33, 0xfb, 0xff, // 7
		0x00, 0x00, 0xf8, 0xff, // 9
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32LSB20);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo20BitIn32BitLSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000
	};

	const tubyte c_expectIntegerSamplesA[20] = {
		0xff, 0xff, 0x07, 0x00, // 0
		0xcd, 0xcc, 0x04, 0x00, // 2
		0x9a, 0x99, 0x01, 0x00, // 4
		0xcd, 0xcc, 0xfc, 0xff, // 6
		0x9a, 0x99, 0xf9, 0xff, // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x66, 0x66, 0x06, 0x00, // 1
		0x33, 0x33, 0x03, 0x00, // 3
		0x66, 0x66, 0xfe, 0xff, // 5
		0x33, 0x33, 0xfb, 0xff, // 7
		0x00, 0x00, 0xf8, 0xff, // 9
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32LSB20);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo20BitIn32BitMSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0: = 0x0007FFFF =  524287 =  1048574 =  1048574.0 / 1048574.0 =  1.0
		 0.8, //  1: = 0x00066666 =  419430 =   838860 =   838859.2 / 1048574.0 =  0.8 (  838859.2  838858=1.2  838860=0.8 )
		 0.6, //  2: = 0x0004CCCC =  314572 =   629144 =   629144.4 / 1048574.0 =  0.6 (  629144.4  629144=0.4  629146=1.6 )
		 0.4, //  3: = 0x00033333 =  209715 =   419430 =   419429.6 / 1048574.0 =  0.4 (  419429.6  419428=1.6  419430=0.4 )
		 0.2, //  4: = 0x00019999 =  104857 =   209714 =   209714.8 / 1048574.0 =  0.2 (  209714.8  209714=0.8  209716=1.2 )
		-0.2, //  5: = 0xFFFE6666 = -104858 =  -209716 =  -209715.2 / 1048576.0 = -0.2 ( -209715.2 -209714=1.2 -209716=0.8 )
		-0.4, //  6: = 0xFFFCCCCD = -209715 =  -419430 =  -419430.4 / 1048576.0 = -0.4 ( -419430.4 -419430=0.4 -419432=1.6 )
		-0.6, //  7: = 0xFFFB3333 = -314573 =  -629146 =  -629145.6 / 1048576.0 = -0.6 ( -629145.6 -629144=1.6 -629146=0.4 )
		-0.8, //  8: = 0xFFF9999A = -419430 =  -838860 =  -838860.8 / 1048576.0 = -0.8 ( -838860.8 -838860=0.8 -838862=1.2 )
		-1.0, //  9: = 0xFFF80000 = -524288 = -1048576 = -1048576.0 / 1048576.0 = -1.0
		 1.2, // 10: = 0x0007FFFF =  1.2
		-1.2  // 11: = 0xFFF80000 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamples[48] = {
		0x00, 0x07, 0xff, 0xff, // 0
		0x00, 0x06, 0x66, 0x66, // 1
		0x00, 0x04, 0xcc, 0xcc, // 2
		0x00, 0x03, 0x33, 0x33, // 3
		0x00, 0x01, 0x99, 0x99, // 4
		0xff, 0xfe, 0x66, 0x66, // 5
		0xff, 0xfc, 0xcc, 0xcd, // 6
		0xff, 0xfb, 0x33, 0x33, // 7
		0xff, 0xf9, 0x99, 0x9a, // 8
		0xff, 0xf8, 0x00, 0x00, // 9
		0x00, 0x07, 0xff, 0xff, // 10
		0xff, 0xf8, 0x00, 0x00  // 11
	};

	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTInt32MSB20);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamples,48));
}

TEST(ASIOData,sampleTo20BitIn32BitMSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0: = 0x0007FFFF =  524287 =  1048574 =  1048574.0 / 1048574.0 =  1.0
		 0.8, //  1: = 0x00066666 =  419430 =   838860 =   838859.2 / 1048574.0 =  0.8 (  838859.2  838858=1.2  838860=0.8 )
		 0.6, //  2: = 0x0004CCCC =  314572 =   629144 =   629144.4 / 1048574.0 =  0.6 (  629144.4  629144=0.4  629146=1.6 )
		 0.4, //  3: = 0x00033333 =  209715 =   419430 =   419429.6 / 1048574.0 =  0.4 (  419429.6  419428=1.6  419430=0.4 )
		 0.2, //  4: = 0x00019999 =  104857 =   209714 =   209714.8 / 1048574.0 =  0.2 (  209714.8  209714=0.8  209716=1.2 )
		-0.2, //  5: = 0xFFFE6666 = -104858 =  -209716 =  -209715.2 / 1048576.0 = -0.2 ( -209715.2 -209714=1.2 -209716=0.8 )
		-0.4, //  6: = 0xFFFCCCCD = -209715 =  -419430 =  -419430.4 / 1048576.0 = -0.4 ( -419430.4 -419430=0.4 -419432=1.6 )
		-0.6, //  7: = 0xFFFB3333 = -314573 =  -629146 =  -629145.6 / 1048576.0 = -0.6 ( -629145.6 -629144=1.6 -629146=0.4 )
		-0.8, //  8: = 0xFFF9999A = -419430 =  -838860 =  -838860.8 / 1048576.0 = -0.8 ( -838860.8 -838860=0.8 -838862=1.2 )
		-1.0, //  9: = 0xFFF80000 = -524288 = -1048576 = -1048576.0 / 1048576.0 = -1.0
		 1.2, // 10: = 0x0007FFFF =  1.2
		-1.2  // 11: = 0xFFF80000 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[24] = {
		0x00, 0x07, 0xff, 0xff, // 0
		0x00, 0x04, 0xcc, 0xcc, // 2
		0x00, 0x01, 0x99, 0x99, // 4
		0xff, 0xfc, 0xcc, 0xcd, // 6
		0xff, 0xf9, 0x99, 0x9a, // 8
		0x00, 0x07, 0xff, 0xff  // 10
	};

	const tubyte c_expectIntegerSamplesB[24] = {
		0x00, 0x06, 0x66, 0x66, // 1
		0x00, 0x03, 0x33, 0x33, // 3
		0xff, 0xfe, 0x66, 0x66, // 5
		0xff, 0xfb, 0x33, 0x33, // 7
		0xff, 0xf8, 0x00, 0x00, // 9
		0xff, 0xf8, 0x00, 0x00  // 11
	};

	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt32MSB20);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,24));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,24));
}

TEST(ASIOData,sampleTo20BitIn32BitMSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0: = 0x0007FFFF =  524287 =  1048574 =  1048574.0 / 1048574.0 =  1.0
		 0.8, //  1: = 0x00066666 =  419430 =   838860 =   838859.2 / 1048574.0 =  0.8 (  838859.2  838858=1.2  838860=0.8 )
		 0.6, //  2: = 0x0004CCCC =  314572 =   629144 =   629144.4 / 1048574.0 =  0.6 (  629144.4  629144=0.4  629146=1.6 )
		 0.4, //  3: = 0x00033333 =  209715 =   419430 =   419429.6 / 1048574.0 =  0.4 (  419429.6  419428=1.6  419430=0.4 )
		 0.2, //  4: = 0x00019999 =  104857 =   209714 =   209714.8 / 1048574.0 =  0.2 (  209714.8  209714=0.8  209716=1.2 )
		-0.2, //  5: = 0xFFFE6666 = -104858 =  -209716 =  -209715.2 / 1048576.0 = -0.2 ( -209715.2 -209714=1.2 -209716=0.8 )
		-0.4, //  6: = 0xFFFCCCCD = -209715 =  -419430 =  -419430.4 / 1048576.0 = -0.4 ( -419430.4 -419430=0.4 -419432=1.6 )
		-0.6, //  7: = 0xFFFB3333 = -314573 =  -629146 =  -629145.6 / 1048576.0 = -0.6 ( -629145.6 -629144=1.6 -629146=0.4 )
		-0.8, //  8: = 0xFFF9999A = -419430 =  -838860 =  -838860.8 / 1048576.0 = -0.8 ( -838860.8 -838860=0.8 -838862=1.2 )
		-1.0, //  9: = 0xFFF80000 = -524288 = -1048576 = -1048576.0 / 1048576.0 = -1.0
		 1.2, // 10: = 0x0007FFFF =  1.2
		-1.2  // 11: = 0xFFF80000 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[12] = {
		0x00, 0x07, 0xff, 0xff, // 0
		0x00, 0x01, 0x99, 0x99, // 4
		0xff, 0xf9, 0x99, 0x9a  // 8
	};
	
	const tubyte c_expectIntegerSamplesB[12] = {
		0x00, 0x06, 0x66, 0x66, // 1
		0xff, 0xfe, 0x66, 0x66, // 5
		0xff, 0xf8, 0x00, 0x00  // 9
	};
	
	const tubyte c_expectIntegerSamplesC[12] = {
		0x00, 0x04, 0xcc, 0xcc, // 2
		0xff, 0xfc, 0xcc, 0xcd, // 6
		0x00, 0x07, 0xff, 0xff  // 10
	};
	
	const tubyte c_expectIntegerSamplesD[12] = {
		0x00, 0x03, 0x33, 0x33, // 3
		0xff, 0xfb, 0x33, 0x33, // 7
		0xff, 0xf8, 0x00, 0x00  // 11
	};

	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTInt32MSB20);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,12));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,12));
	ASSERT_EQ(0,memcmp(data.asioData(2,0),c_expectIntegerSamplesC,12));
	ASSERT_EQ(0,memcmp(data.asioData(3,0),c_expectIntegerSamplesD,12));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo20BitIn32BitMSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};

	const tubyte c_expectIntegerSamplesA[20] = {
		0x00, 0x07, 0xff, 0xf0, // 0
		0x00, 0x04, 0xcc, 0xc0, // 2
		0x00, 0x01, 0x99, 0x90, // 4
		0xff, 0xfc, 0xcc, 0xd0, // 6
		0xff, 0xf9, 0x99, 0xa0  // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x00, 0x06, 0x66, 0x60, // 1
		0x00, 0x03, 0x33, 0x30, // 3
		0xff, 0xfe, 0x66, 0x60, // 5
		0xff, 0xfb, 0x33, 0x30, // 7
		0xff, 0xf8, 0x00, 0x00  // 9
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB20);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo20BitIn32BitMSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tubyte c_expectIntegerSamplesA[20] = {
		0x00, 0x07, 0xff, 0xff, // 0
		0x00, 0x04, 0xcc, 0xcd, // 2
		0x00, 0x01, 0x99, 0x9a, // 4
		0xff, 0xfc, 0xcc, 0xcd, // 6
		0xff, 0xf9, 0x99, 0x9a, // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x00, 0x06, 0x66, 0x66, // 1
		0x00, 0x03, 0x33, 0x33, // 3
		0xff, 0xfe, 0x66, 0x66, // 5
		0xff, 0xfb, 0x33, 0x33, // 7
		0xff, 0xf8, 0x00, 0x00, // 9
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB20);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo20BitIn32BitMSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000
	};
	
	const tubyte c_expectIntegerSamplesA[20] = {
		0x00, 0x07, 0xff, 0xff, // 0
		0x00, 0x04, 0xcc, 0xcd, // 2
		0x00, 0x01, 0x99, 0x9a, // 4
		0xff, 0xfc, 0xcc, 0xcd, // 6
		0xff, 0xf9, 0x99, 0x9a, // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x00, 0x06, 0x66, 0x66, // 1
		0x00, 0x03, 0x33, 0x33, // 3
		0xff, 0xfe, 0x66, 0x66, // 5
		0xff, 0xfb, 0x33, 0x33, // 7
		0xff, 0xf8, 0x00, 0x00, // 9
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB20);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo24BitIn32BitLSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7FFFFF =  8388607 <-  16777214 =  16777214.0 / 16777214.0 =  1.0
		 0.8, //  1:  0x666666 =  6710886 <-  13421772 =  13421771.2 / 16777214.0 =  0.8 ( 13421771.2  13421770=1.2  13421772=0.8)
		 0.6, //  2:  0x4CCCCC =  5033164 <-  10066328 =  10066328.4 / 16777214.0 =  0.6 ( 10066328.4  10066328=0.4  10066330=1.6)
		 0.4, //  3:  0x333333 =  3355443 <-   6710886 =   6710885.6 / 16777214.0 =  0.4 (  6710885.6   6710884=1.6   6710886=0.4)
		 0.2, //  4:  0x199999 =  1677721 <-   3355442 =   3355442.8 / 16777214.0 =  0.2 (  3355442.8   3355442=0.8   3355444=1.2)
		-0.2, //  5:  0xE66666 = -1677722 <-  -3355444 =  -3355443.2 / 16777216.0 = -0.2 ( -3355443.2  -3355442=1.2  -3355444=0.8)
		-0.4, //  6:  0xCCCCCD = -3355443 <-  -6710886 =  -6710886.4 / 16777216.0 = -0.4 ( -6710886.4  -6710886=0.4  -6710888=1.6)
		-0.6, //  7:  0xB33333 = -5033165 <- -10066330 = -10066329.6 / 16777216.0 = -0.6 (-10066329.6 -10066328=1.6 -10066330=0.4)
		-0.8, //  8:  0x99999A = -6710886 <- -13421772 = -13421772.8 / 16777216.0 = -0.8 (-13421772.8 -13421772=0.8 -13421774=1.2)
		-1.0, //  9:  0x800000 = -8388608 <- -16777216 = -16777216.0 / 16777216.0 = -1.0
		 1.2, // 10:  0x7FFFFF =  8388607 =  1.2
		-1.2  // 11:  0x800000 = -8388608 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamples[48] = {
		0xff, 0xff, 0x7f, 0x00, // 0
		0x66, 0x66, 0x66, 0x00, // 1
		0xcc, 0xcc, 0x4c, 0x00, // 2
		0x33, 0x33, 0x33, 0x00, // 3
		0x99, 0x99, 0x19, 0x00, // 4
		0x66, 0x66, 0xe6, 0xff, // 5
		0xcd, 0xcc, 0xcc, 0xff, // 6
		0x33, 0x33, 0xb3, 0xff, // 7
		0x9a, 0x99, 0x99, 0xff, // 8
		0x00, 0x00, 0x80, 0xff, // 9
		0xff, 0xff, 0x7f, 0x00, // 10
		0x00, 0x00, 0x80, 0xff  // 11
	};

	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTInt32LSB24);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamples,48));
}

TEST(ASIOData,sampleTo24BitIn32BitLSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7FFFFF =  8388607 <-  16777214 =  16777214.0 / 16777214.0 =  1.0
		 0.8, //  1:  0x666666 =  6710886 <-  13421772 =  13421771.2 / 16777214.0 =  0.8 ( 13421771.2  13421770=1.2  13421772=0.8)
		 0.6, //  2:  0x4CCCCC =  5033164 <-  10066328 =  10066328.4 / 16777214.0 =  0.6 ( 10066328.4  10066328=0.4  10066330=1.6)
		 0.4, //  3:  0x333333 =  3355443 <-   6710886 =   6710885.6 / 16777214.0 =  0.4 (  6710885.6   6710884=1.6   6710886=0.4)
		 0.2, //  4:  0x199999 =  1677721 <-   3355442 =   3355442.8 / 16777214.0 =  0.2 (  3355442.8   3355442=0.8   3355444=1.2)
		-0.2, //  5:  0xE66666 = -1677722 <-  -3355444 =  -3355443.2 / 16777216.0 = -0.2 ( -3355443.2  -3355442=1.2  -3355444=0.8)
		-0.4, //  6:  0xCCCCCD = -3355443 <-  -6710886 =  -6710886.4 / 16777216.0 = -0.4 ( -6710886.4  -6710886=0.4  -6710888=1.6)
		-0.6, //  7:  0xB33333 = -5033165 <- -10066330 = -10066329.6 / 16777216.0 = -0.6 (-10066329.6 -10066328=1.6 -10066330=0.4)
		-0.8, //  8:  0x99999A = -6710886 <- -13421772 = -13421772.8 / 16777216.0 = -0.8 (-13421772.8 -13421772=0.8 -13421774=1.2)
		-1.0, //  9:  0x800000 = -8388608 <- -16777216 = -16777216.0 / 16777216.0 = -1.0
		 1.2, // 10:  0x7FFFFF =  8388607 =  1.2
		-1.2  // 11:  0x800000 = -8388608 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[24] = {
		0xff, 0xff, 0x7f, 0x00, // 0
		0xcc, 0xcc, 0x4c, 0x00, // 2
		0x99, 0x99, 0x19, 0x00, // 4
		0xcd, 0xcc, 0xcc, 0xff, // 6
		0x9a, 0x99, 0x99, 0xff, // 8
		0xff, 0xff, 0x7f, 0x00  // 10
	};

	const tubyte c_expectIntegerSamplesB[24] = {
		0x66, 0x66, 0x66, 0x00, // 1
		0x33, 0x33, 0x33, 0x00, // 3
		0x66, 0x66, 0xe6, 0xff, // 5
		0x33, 0x33, 0xb3, 0xff, // 7
		0x00, 0x00, 0x80, 0xff, // 9
		0x00, 0x00, 0x80, 0xff  // 11
	};

	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt32LSB24);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,24));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,24));
}

TEST(ASIOData,sampleTo24BitIn32BitLSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7FFFFF =  8388607 <-  16777214 =  16777214.0 / 16777214.0 =  1.0
		 0.8, //  1:  0x666666 =  6710886 <-  13421772 =  13421771.2 / 16777214.0 =  0.8 ( 13421771.2  13421770=1.2  13421772=0.8)
		 0.6, //  2:  0x4CCCCC =  5033164 <-  10066328 =  10066328.4 / 16777214.0 =  0.6 ( 10066328.4  10066328=0.4  10066330=1.6)
		 0.4, //  3:  0x333333 =  3355443 <-   6710886 =   6710885.6 / 16777214.0 =  0.4 (  6710885.6   6710884=1.6   6710886=0.4)
		 0.2, //  4:  0x199999 =  1677721 <-   3355442 =   3355442.8 / 16777214.0 =  0.2 (  3355442.8   3355442=0.8   3355444=1.2)
		-0.2, //  5:  0xE66666 = -1677722 <-  -3355444 =  -3355443.2 / 16777216.0 = -0.2 ( -3355443.2  -3355442=1.2  -3355444=0.8)
		-0.4, //  6:  0xCCCCCD = -3355443 <-  -6710886 =  -6710886.4 / 16777216.0 = -0.4 ( -6710886.4  -6710886=0.4  -6710888=1.6)
		-0.6, //  7:  0xB33333 = -5033165 <- -10066330 = -10066329.6 / 16777216.0 = -0.6 (-10066329.6 -10066328=1.6 -10066330=0.4)
		-0.8, //  8:  0x99999A = -6710886 <- -13421772 = -13421772.8 / 16777216.0 = -0.8 (-13421772.8 -13421772=0.8 -13421774=1.2)
		-1.0, //  9:  0x800000 = -8388608 <- -16777216 = -16777216.0 / 16777216.0 = -1.0
		 1.2, // 10:  0x7FFFFF =  8388607 =  1.2
		-1.2  // 11:  0x800000 = -8388608 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[12] = {
		0xff, 0xff, 0x7f, 0x00, // 0
		0x99, 0x99, 0x19, 0x00, // 4
		0x9a, 0x99, 0x99, 0xff, // 8
	};

	const tubyte c_expectIntegerSamplesB[12] = {
		0x66, 0x66, 0x66, 0x00, // 1
		0x66, 0x66, 0xe6, 0xff, // 5
		0x00, 0x00, 0x80, 0xff, // 9
	};

	const tubyte c_expectIntegerSamplesC[12] = {
		0xcc, 0xcc, 0x4c, 0x00, // 2
		0xcd, 0xcc, 0xcc, 0xff, // 6
		0xff, 0xff, 0x7f, 0x00, // 10
	};

	const tubyte c_expectIntegerSamplesD[12] = {
		0x33, 0x33, 0x33, 0x00, // 3
		0x33, 0x33, 0xb3, 0xff, // 7
		0x00, 0x00, 0x80, 0xff  // 11
	};

	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTInt32LSB24);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,12));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,12));
	ASSERT_EQ(0,memcmp(data.asioData(2,0),c_expectIntegerSamplesC,12));
	ASSERT_EQ(0,memcmp(data.asioData(3,0),c_expectIntegerSamplesD,12));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo24BitIn32BitLSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};

	const tubyte c_expectIntegerSamplesA[20] = {
		0x00, 0xff, 0x7f, 0x00, // 0
		0x00, 0xcc, 0x4c, 0x00, // 2
		0x00, 0x99, 0x19, 0x00, // 4
		0x00, 0xcd, 0xcc, 0xff, // 6
		0x00, 0x9a, 0x99, 0xff  // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x00, 0x66, 0x66, 0x00, // 1
		0x00, 0x33, 0x33, 0x00, // 3
		0x00, 0x66, 0xe6, 0xff, // 5
		0x00, 0x33, 0xb3, 0xff, // 7
		0x00, 0x00, 0x80, 0xff  // 9
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32LSB24);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo24BitIn32BitLSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tubyte c_expectIntegerSamplesA[20] = {
		0xff, 0xff, 0x7f, 0x00, // 0
		0xcc, 0xcc, 0x4c, 0x00, // 2
		0x99, 0x99, 0x19, 0x00, // 4
		0xcd, 0xcc, 0xcc, 0xff, // 6
		0x9a, 0x99, 0x99, 0xff, // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x66, 0x66, 0x66, 0x00, // 1
		0x33, 0x33, 0x33, 0x00, // 3
		0x66, 0x66, 0xe6, 0xff, // 5
		0x33, 0x33, 0xb3, 0xff, // 7
		0x00, 0x00, 0x80, 0xff  // 9
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32LSB24);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo24BitIn32BitLSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
	};

	const tubyte c_expectIntegerSamplesA[20] = {
		0xff, 0xff, 0x7f, 0x00, // 0
		0xcd, 0xcc, 0x4c, 0x00, // 2
		0x9a, 0x99, 0x19, 0x00, // 4
		0xcd, 0xcc, 0xcc, 0xff, // 6
		0x9a, 0x99, 0x99, 0xff, // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x66, 0x66, 0x66, 0x00, // 1
		0x33, 0x33, 0x33, 0x00, // 3
		0x66, 0x66, 0xe6, 0xff, // 5
		0x33, 0x33, 0xb3, 0xff, // 7
		0x00, 0x00, 0x80, 0xff, // 9
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32LSB24);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo24BitIn32BitMSBConvertionMono)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7FFFFF =  8388607 <-  16777214 =  16777214.0 / 16777214.0 =  1.0
		 0.8, //  1:  0x666666 =  6710886 <-  13421772 =  13421771.2 / 16777214.0 =  0.8 ( 13421771.2  13421770=1.2  13421772=0.8)
		 0.6, //  2:  0x4CCCCC =  5033164 <-  10066328 =  10066328.4 / 16777214.0 =  0.6 ( 10066328.4  10066328=0.4  10066330=1.6)
		 0.4, //  3:  0x333333 =  3355443 <-   6710886 =   6710885.6 / 16777214.0 =  0.4 (  6710885.6   6710884=1.6   6710886=0.4)
		 0.2, //  4:  0x199999 =  1677721 <-   3355442 =   3355442.8 / 16777214.0 =  0.2 (  3355442.8   3355442=0.8   3355444=1.2)
		-0.2, //  5:  0xE66666 = -1677722 <-  -3355444 =  -3355443.2 / 16777216.0 = -0.2 ( -3355443.2  -3355442=1.2  -3355444=0.8)
		-0.4, //  6:  0xCCCCCD = -3355443 <-  -6710886 =  -6710886.4 / 16777216.0 = -0.4 ( -6710886.4  -6710886=0.4  -6710888=1.6)
		-0.6, //  7:  0xB33333 = -5033165 <- -10066330 = -10066329.6 / 16777216.0 = -0.6 (-10066329.6 -10066328=1.6 -10066330=0.4)
		-0.8, //  8:  0x99999A = -6710886 <- -13421772 = -13421772.8 / 16777216.0 = -0.8 (-13421772.8 -13421772=0.8 -13421774=1.2)
		-1.0, //  9:  0x800000 = -8388608 <- -16777216 = -16777216.0 / 16777216.0 = -1.0
		 1.2, // 10:  0x7FFFFF =  8388607 =  1.2
		-1.2  // 11:  0x800000 = -8388608 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamples[48] = {
		0x00, 0x7f, 0xff, 0xff, // 0
		0x00, 0x66, 0x66, 0x66, // 1
		0x00, 0x4c, 0xcc, 0xcc, // 2
		0x00, 0x33, 0x33, 0x33, // 3
		0x00, 0x19, 0x99, 0x99, // 4
		0xff, 0xe6, 0x66, 0x66, // 5
		0xff, 0xcc, 0xcc, 0xcd, // 6
		0xff, 0xb3, 0x33, 0x33, // 7
		0xff, 0x99, 0x99, 0x9a, // 8
		0xff, 0x80, 0x00, 0x00, // 9
		0x00, 0x7f, 0xff, 0xff, // 10
		0xff, 0x80, 0x00, 0x00  // 11
	};

	ASIOData data(12,1,1);
	data.setSampleType(ASIOSTInt32MSB24);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 12;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamples,48));
}

TEST(ASIOData,sampleTo24BitIn32BitMSBConvertionStereo)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7FFFFF =  8388607 <-  16777214 =  16777214.0 / 16777214.0 =  1.0
		 0.8, //  1:  0x666666 =  6710886 <-  13421772 =  13421771.2 / 16777214.0 =  0.8 ( 13421771.2  13421770=1.2  13421772=0.8)
		 0.6, //  2:  0x4CCCCC =  5033164 <-  10066328 =  10066328.4 / 16777214.0 =  0.6 ( 10066328.4  10066328=0.4  10066330=1.6)
		 0.4, //  3:  0x333333 =  3355443 <-   6710886 =   6710885.6 / 16777214.0 =  0.4 (  6710885.6   6710884=1.6   6710886=0.4)
		 0.2, //  4:  0x199999 =  1677721 <-   3355442 =   3355442.8 / 16777214.0 =  0.2 (  3355442.8   3355442=0.8   3355444=1.2)
		-0.2, //  5:  0xE66666 = -1677722 <-  -3355444 =  -3355443.2 / 16777216.0 = -0.2 ( -3355443.2  -3355442=1.2  -3355444=0.8)
		-0.4, //  6:  0xCCCCCD = -3355443 <-  -6710886 =  -6710886.4 / 16777216.0 = -0.4 ( -6710886.4  -6710886=0.4  -6710888=1.6)
		-0.6, //  7:  0xB33333 = -5033165 <- -10066330 = -10066329.6 / 16777216.0 = -0.6 (-10066329.6 -10066328=1.6 -10066330=0.4)
		-0.8, //  8:  0x99999A = -6710886 <- -13421772 = -13421772.8 / 16777216.0 = -0.8 (-13421772.8 -13421772=0.8 -13421774=1.2)
		-1.0, //  9:  0x800000 = -8388608 <- -16777216 = -16777216.0 / 16777216.0 = -1.0
		 1.2, // 10:  0x7FFFFF =  8388607 =  1.2
		-1.2  // 11:  0x800000 = -8388608 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[24] = {
		0x00, 0x7f, 0xff, 0xff, // 0
		0x00, 0x4c, 0xcc, 0xcc, // 2
		0x00, 0x19, 0x99, 0x99, // 4
		0xff, 0xcc, 0xcc, 0xcd, // 6
		0xff, 0x99, 0x99, 0x9a, // 8
		0x00, 0x7f, 0xff, 0xff // 10
	};

	const tubyte c_expectIntegerSamplesB[24] = {
		0x00, 0x66, 0x66, 0x66, // 1
		0x00, 0x33, 0x33, 0x33, // 3
		0xff, 0xe6, 0x66, 0x66, // 5
		0xff, 0xb3, 0x33, 0x33, // 7
		0xff, 0x80, 0x00, 0x00, // 9
		0xff, 0x80, 0x00, 0x00  // 11
	};

	ASIOData data(6,2,2);
	data.setSampleType(ASIOSTInt32MSB24);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 6;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,24));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,24));
}

TEST(ASIOData,sampleTo24BitIn32BitMSBConvertionQuadrophonic)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_sampleInput[12] = {
		1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
	};
#else
	const sample_t c_sampleInput[12] = {
		 1.0, //  0:  0x7FFFFF =  8388607 <-  16777214 =  16777214.0 / 16777214.0 =  1.0
		 0.8, //  1:  0x666666 =  6710886 <-  13421772 =  13421771.2 / 16777214.0 =  0.8 ( 13421771.2  13421770=1.2  13421772=0.8)
		 0.6, //  2:  0x4CCCCC =  5033164 <-  10066328 =  10066328.4 / 16777214.0 =  0.6 ( 10066328.4  10066328=0.4  10066330=1.6)
		 0.4, //  3:  0x333333 =  3355443 <-   6710886 =   6710885.6 / 16777214.0 =  0.4 (  6710885.6   6710884=1.6   6710886=0.4)
		 0.2, //  4:  0x199999 =  1677721 <-   3355442 =   3355442.8 / 16777214.0 =  0.2 (  3355442.8   3355442=0.8   3355444=1.2)
		-0.2, //  5:  0xE66666 = -1677722 <-  -3355444 =  -3355443.2 / 16777216.0 = -0.2 ( -3355443.2  -3355442=1.2  -3355444=0.8)
		-0.4, //  6:  0xCCCCCD = -3355443 <-  -6710886 =  -6710886.4 / 16777216.0 = -0.4 ( -6710886.4  -6710886=0.4  -6710888=1.6)
		-0.6, //  7:  0xB33333 = -5033165 <- -10066330 = -10066329.6 / 16777216.0 = -0.6 (-10066329.6 -10066328=1.6 -10066330=0.4)
		-0.8, //  8:  0x99999A = -6710886 <- -13421772 = -13421772.8 / 16777216.0 = -0.8 (-13421772.8 -13421772=0.8 -13421774=1.2)
		-1.0, //  9:  0x800000 = -8388608 <- -16777216 = -16777216.0 / 16777216.0 = -1.0
		 1.2, // 10:  0x7FFFFF =  8388607 =  1.2
		-1.2  // 11:  0x800000 = -8388608 = -1.2
	};
#endif
	
	const tubyte c_expectIntegerSamplesA[12] = {
		0x00, 0x7f, 0xff, 0xff, // 0
		0x00, 0x19, 0x99, 0x99, // 4
		0xff, 0x99, 0x99, 0x9a  // 8
	};

	const tubyte c_expectIntegerSamplesB[12] = {
		0x00, 0x66, 0x66, 0x66, // 1
		0xff, 0xe6, 0x66, 0x66, // 5
		0xff, 0x80, 0x00, 0x00  // 9
	};

	const tubyte c_expectIntegerSamplesC[12] = {
		0x00, 0x4c, 0xcc, 0xcc, // 2
		0xff, 0xcc, 0xcc, 0xcd, // 6
		0x00, 0x7f, 0xff, 0xff  // 10
	};

	const tubyte c_expectIntegerSamplesD[12] = {
		0x00, 0x33, 0x33, 0x33, // 3
		0xff, 0xb3, 0x33, 0x33, // 7
		0xff, 0x80, 0x00, 0x00  // 11
	};

	ASIOData data(3,4,4);
	data.setSampleType(ASIOSTInt32MSB24);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));
	part.length() = 3;
	part.done() = true;
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,12));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,12));
	ASSERT_EQ(0,memcmp(data.asioData(2,0),c_expectIntegerSamplesC,12));
	ASSERT_EQ(0,memcmp(data.asioData(3,0),c_expectIntegerSamplesD,12));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo24BitIn32BitMSBConvertionStereoInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};

	const tubyte c_expectIntegerSamplesA[20] = {
		0x00, 0x7f, 0xff, 0x00, // 0
		0x00, 0x4c, 0xcc, 0x00, // 2
		0x00, 0x19, 0x99, 0x00, // 4
		0xff, 0xcc, 0xcd, 0x00, // 6
		0xff, 0x99, 0x9a, 0x00, // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x00, 0x66, 0x66, 0x00, // 1
		0x00, 0x33, 0x33, 0x00, // 3
		0xff, 0xe6, 0x66, 0x00, // 5
		0xff, 0xb3, 0x33, 0x00, // 7
		0xff, 0x80, 0x00, 0x00  // 9
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB24);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo24BitIn32BitMSBConvertionStereoInt24)
{
	const tuint32 c_sampleInput[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tubyte c_expectIntegerSamplesA[20] = {
		0x00, 0x7f, 0xff, 0xff, // 0
		0x00, 0x4c, 0xcc, 0xcc, // 2
		0x00, 0x19, 0x99, 0x99, // 4
		0xff, 0xcc, 0xcc, 0xcd, // 6
		0xff, 0x99, 0x99, 0x9a, // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x00, 0x66, 0x66, 0x66, // 1
		0x00, 0x33, 0x33, 0x33, // 3
		0xff, 0xe6, 0x66, 0x66, // 5
		0xff, 0xb3, 0x33, 0x33, // 7
		0xff, 0x80, 0x00, 0x00  // 9
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB24);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

TEST(ASIOData,sampleTo24BitIn32BitMSBConvertionStereoInt32)
{
	const tuint32 c_sampleInput[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
	};

	const tubyte c_expectIntegerSamplesA[20] = {
		0x00, 0x7f, 0xff, 0xff, // 0
		0x00, 0x4c, 0xcc, 0xcd, // 2
		0x00, 0x19, 0x99, 0x9a, // 4
		0xff, 0xcc, 0xcc, 0xcd, // 6
		0xff, 0x99, 0x99, 0x9a, // 8
	};

	const tubyte c_expectIntegerSamplesB[20] = {
		0x00, 0x66, 0x66, 0x66, // 1
		0x00, 0x33, 0x33, 0x33, // 3
		0xff, 0xe6, 0x66, 0x66, // 5
		0xff, 0xb3, 0x33, 0x33, // 7
		0xff, 0x80, 0x00, 0x00  // 9
	};

	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB24);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),c_sampleInput,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),c_expectIntegerSamplesA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),c_expectIntegerSamplesB,20));
}

//-------------------------------------------------------------------------------------------

void testVolumeWithInt16(sample_t vol, const tuint16 in[10], const tubyte expectA[20], const tubyte expectB[20])
{
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB);
	data.setVolume(vol);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),in,10 * sizeof(tint16));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt16);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),expectA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),expectB,20));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,volume32BitsMSBWithInt16)
{
	const tuint16 c_sampleInput[10] = {
		0x7fff, 
		0x6666, 
		0x4ccc, 
		0x3333, 
		0x1999,
		0xe667, 
		0xcccd, 
		0xb334, 
		0x999a, 
		0x8001
	};

    const tubyte c_expectIntegerSamplesA_75Percent[20] = {
		0x5F, 0xFF, 0x40, 0x00,
		0x39, 0x99, 0x00, 0x00,
		0x13, 0x32, 0xC0, 0x00,
		0xD9, 0x99, 0xC0, 0x00,
		0xB3, 0x33, 0x80, 0x00
	};

    const tubyte c_expectIntegerSamplesB_75Percent[20] = {
		0x4C, 0xCC, 0x80, 0x00,
		0x26, 0x66, 0x40, 0x00,
		0xEC, 0xCD, 0x40, 0x00,
		0xC6, 0x67, 0x00, 0x00,
		0xA0, 0x00, 0xC0, 0x00
	};

    const tubyte c_expectIntegerSamplesA_25Percent[20] = {
		0x1F, 0xFF, 0xC0, 0x00,
		0x13, 0x33, 0x00, 0x00,
		0x06, 0x66, 0x40, 0x00,
		0xF3, 0x33, 0x40, 0x00,
		0xE6, 0x66, 0x80, 0x00,
	};

    const tubyte c_expectIntegerSamplesB_25Percent[20] = {
		0x19, 0x99, 0x80, 0x00,
		0x0C, 0xCC, 0xC0, 0x00,
		0xF9, 0x99, 0xC0, 0x00,
		0xEC, 0xCD, 0x00, 0x00,
		0xE0, 0x00, 0x40, 0x00,
	};
	
	testVolumeWithInt16(0.75, c_sampleInput, c_expectIntegerSamplesA_75Percent, c_expectIntegerSamplesB_75Percent);
	testVolumeWithInt16(0.25, c_sampleInput, c_expectIntegerSamplesA_25Percent, c_expectIntegerSamplesB_25Percent);
}

//-------------------------------------------------------------------------------------------

void testVolumeWithInt24(sample_t vol, const tuint32 in[10], const tubyte expectA[20], const tubyte expectB[20])
{
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB);
	data.setVolume(vol);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),in,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt24);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),expectA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),expectB,20));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,volume32BitsMSBWithInt24)
{
	const tuint32 c_int24Samples[10] = {
		0x007fffff,
		0x00666666,
		0x004ccccc,
		0x00333333,
		0x00199999,
		0xffe66667,
		0xffcccccd,
		0xffb33334,
		0xff99999a,
		0xff800000
	};
	
    const tubyte c_expectOutput24A_75Percent[20] = {
		0x5F, 0xFF, 0xFF, 0x40,
		0x39, 0x99, 0x99, 0x00,
		0x13, 0x33, 0x32, 0xC0,
		0xD9, 0x99, 0x99, 0xC0,
		0xB3, 0x33, 0x33, 0x80
	};

    const tubyte c_expectOutput24B_75Percent[20] = {
		0x4C, 0xCC, 0xCC, 0x80,
		0x26, 0x66, 0x66, 0x40,
		0xEC, 0xCC, 0xCD, 0x40,
		0xC6, 0x66, 0x67, 0x00,
		0xA0, 0x00, 0x00, 0x00
	};
	
    const tubyte c_expectOutput24A_25Percent[20] = {
		0x1F, 0xFF, 0xFF, 0xC0,
		0x13, 0x33, 0x33, 0x00,
		0x06, 0x66, 0x66, 0x40,
		0xF3, 0x33, 0x33, 0x40,
		0xE6, 0x66, 0x66, 0x80
	};

    const tubyte c_expectOutput24B_25Percent[20] = {
		0x19, 0x99, 0x99, 0x80,
		0x0C, 0xCC, 0xCC, 0xC0,
		0xF9, 0x99, 0x99, 0xC0,
		0xEC, 0xCC, 0xCD, 0x00,
		0xE0, 0x00, 0x00, 0x00
	};

	testVolumeWithInt24(0.75, c_int24Samples, c_expectOutput24A_75Percent, c_expectOutput24B_75Percent);
	testVolumeWithInt24(0.25, c_int24Samples, c_expectOutput24A_25Percent, c_expectOutput24B_25Percent);
}

//-------------------------------------------------------------------------------------------

void testVolumeWithInt32(sample_t vol, const tuint32 in[10], const tubyte expectA[20], const tubyte expectB[20])
{
	ASIOData data(5,2,2);
	data.setSampleType(ASIOSTInt32MSB);
	data.setVolume(vol);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0),in,10 * sizeof(tint32));
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleInt32);
	
	data.convert();
	
	ASSERT_EQ(0,memcmp(data.asioData(0,0),expectA,20));
	ASSERT_EQ(0,memcmp(data.asioData(1,0),expectB,20));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,volume32BitsMSBWithInt32)
{
	const tuint32 c_int32Samples[10] = {
		0x7fffffff,
		0x66666666,
		0x4ccccccc,
		0x33333333,
		0x19999999,
		0xe6666667,
		0xcccccccd,
		0xb3333334,
		0x9999999a,
		0x80000000
	};
	
    const tubyte c_expectOutput32A_75Percent[20] = {
		0x5F, 0xFF, 0xFF, 0xFF,
		0x39, 0x99, 0x99, 0x99,
		0x13, 0x33, 0x33, 0x33,
		0xD9, 0x99, 0x99, 0x9A,
		0xB3, 0x33, 0x33, 0x34
	};

    const tubyte c_expectOutput32B_75Percent[20] = {
		0x4C, 0xCC, 0xCC, 0xCD,
		0x26, 0x66, 0x66, 0x66,
		0xEC, 0xCC, 0xCC, 0xCD,
		0xC6, 0x66, 0x66, 0x67,
		0xA0, 0x00, 0x00, 0x00
	};

    const tubyte c_expectOutput32A_25Percent[20] = {
		0x20, 0x00, 0x00, 0x00,
		0x13, 0x33, 0x33, 0x33,
		0x06, 0x66, 0x66, 0x66,
		0xF3, 0x33, 0x33, 0x33,
		0xE6, 0x66, 0x66, 0x67
	};

    const tubyte c_expectOutput32B_25Percent[20] = {
		0x19, 0x99, 0x99, 0x9A,
		0x0C, 0xCC, 0xCC, 0xCD,
		0xF9, 0x99, 0x99, 0x9A,
		0xEC, 0xCC, 0xCC, 0xCD,
		0xE0, 0x00, 0x00, 0x00
	};

	testVolumeWithInt32(0.75, c_int32Samples, c_expectOutput32A_75Percent, c_expectOutput32B_75Percent);
	testVolumeWithInt32(0.25, c_int32Samples, c_expectOutput32A_25Percent, c_expectOutput32B_25Percent);
}

//-------------------------------------------------------------------------------------------

void expectCentreLFEData(const sample_t *sample, tint noSamples, tint offset, const sample_t *filterCooefs, tint noCooefs, sample_t *expect)
{
	tint i, j, idx;
	
	for(i = 0; i < noSamples; i++)
	{
		sample_t sC, fC;
		
		fC = 0.0;
		for(j = 0; j < noCooefs; j++)
		{
			idx = i + j - offset;
			if(idx >= 0 && idx < noSamples)
			{
				sC = (sample[(idx * 2) + 0] + sample[(idx * 2) + 1]) / 2.0;
				fC += sC * filterCooefs[noCooefs - (j + 1)];
			}
		}
		sC = (sample[(i * 2) + 0] + sample[(i * 2) + 1]) / 2.0;
		if(sC < -1.0)
			sC = -1.0;
		else if(sC > 1.0)
			sC = 1.0;
		expect[(i * 2) + 0] = sC;
		if(fC < -1.0)
			fC = -1.0;
		else if(fC > 1.0)
			fC = 1.0;
		expect[(i * 2) + 1] = fC;
	}
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData,sampleTo24BitIn32BitForCentreAndLFEWithNoOffset)
{
	const tfloat64 c_tolerance = 0.0000001;
	const tint c_noOfTestSamples = 10;
	const tint c_noChannels = 2;
	const tint c_noCooefs = 6;
	const tint c_offset = 0;

	const sample_t c_sampleInput[c_noOfTestSamples * c_noChannels] = {
		0.1, 0.2, // 0
		0.3, 0.4, // 1
		0.5, 0.6, // 2
		0.7, 0.8, // 3
		0.9, 1.0, // 4
		0.9, 0.8, // 5
		0.7, 0.6, // 6
		0.5, 0.4, // 7
		0.3, 0.2, // 8
		0.1, 0.0, // 9
	};
	const sample_t c_filterCooefs[c_noCooefs] = {-0.19062389, -0.16668996,  0.61590277,  0.61590277, -0.16668996, -0.19062389};
	
	sample_t expectCLFE[c_noOfTestSamples * c_noChannels];
	expectCentreLFEData(c_sampleInput, c_noOfTestSamples, c_offset, c_filterCooefs, c_noCooefs, expectCLFE);
	
	ASIOData expectData(c_noOfTestSamples, 2, 2);
	expectData.setSampleType(ASIOSTInt32LSB24);
	engine::RData::Part& partE = expectData.nextPart();
	memcpy(expectData.partDataOut(0), expectCLFE, c_noOfTestSamples * c_noChannels * sizeof(sample_t));
	partE.length() = c_noOfTestSamples;
	partE.done() = true;
	expectData.convert();
	
	ASIOData data(c_noOfTestSamples, 2, 2);
	data.setSampleType(ASIOSTInt32LSB24);

	engine::RData::Part& part = data.nextPart();
	memcpy(data.partData(0), c_sampleInput, c_noOfTestSamples * c_noChannels * sizeof(sample_t));
	part.length() = c_noOfTestSamples;
	part.done() = true;

	ASSERT_TRUE(data.partDataCenter(0) != NULL);
	
	engine::FIRFilter filterLFE(c_filterCooefs, c_noCooefs);
	filterLFE.setOffset(c_offset);

	filterLFE.process(&data, engine::e_lfeChannelIndex, true);
	
	data.convert();
	
	const tbyte *outC = reinterpret_cast<const tbyte *>(data.asioDataConst(engine::e_centerChannelIndex, 0));
	const tbyte *expectC = reinterpret_cast<const tbyte*>(expectData.asioDataConst(0, 0));
	
	const tbyte *outLFE = reinterpret_cast<const tbyte*>(data.asioDataConst(engine::e_lfeChannelIndex, 0));
	const tbyte *expectLFE = reinterpret_cast<const tbyte*>(expectData.asioDataConst(1, 0));
	
	EXPECT_EQ(memcmp(outC, expectC, c_noOfTestSamples * 4), 0);
	EXPECT_EQ(memcmp(outLFE, expectLFE, c_noOfTestSamples * 4), 0);
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData, dsdLSBSampleToDSDInt8LSB1Stereo)
{
	const tubyte c_sampleInput[80] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, // 0 : L
		0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, // 0 : R
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, // 1 : L
		0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, // 1 : R
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, // 2 : L
		0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, // 2 : R
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, // 3 : L
		0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, // 3 : R
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, // 4 : L
		0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50  // 4 : R
	};
	
	const tubyte c_expectL[40] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, // 0 : L
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, // 1 : L
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, // 2 : L
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, // 3 : L
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, // 4 : L
	};
	
	const tubyte c_expectR[40] = {
		0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, // 0 : R
		0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, // 1 : R
		0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, // 2 : R
		0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, // 3 : R
		0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50  // 4 : R
	};
	
	ASIOData data(5, 2, 2);
	data.setSampleType(ASIOSTDSDInt8LSB1);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0), c_sampleInput, 80);
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleDSD8LSB);
	
	data.convert();
	
	ASSERT_EQ(0, memcmp(data.asioData(0,0), c_expectL, 40));
	ASSERT_EQ(0, memcmp(data.asioData(1,0), c_expectR, 40));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData, dsdMSBSampleToDSDInt8LSB1Stereo)
{
	const tubyte c_sampleInput[80] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, // 0 : L
		0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, // 0 : R
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, // 1 : L
		0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, // 1 : R
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, // 2 : L
		0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, // 2 : R
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, // 3 : L
		0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, // 3 : R
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, // 4 : L
		0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50  // 4 : R
	};
	
	const tubyte c_expectL[40] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, // 0 : L
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, // 1 : L
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, // 2 : L
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, // 3 : L
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, // 4 : L
	};
	
	const tubyte c_expectR[40] = {
		0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, // 0 : R
		0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, // 1 : R
		0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, // 2 : R
		0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, // 3 : R
		0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50  // 4 : R
	};
	
	tubyte expectL[40], expectR[40];
	for(tint i = 0; i < 40; i++)
	{
		expectL[i] = engine::lsb2msb(c_expectL[i]);
		expectR[i] = engine::lsb2msb(c_expectR[i]);
	}
	
	ASIOData data(5, 2, 2);
	data.setSampleType(ASIOSTDSDInt8LSB1);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0), c_sampleInput, 80);
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleDSD8MSB);
	
	data.convert();
	
	ASSERT_EQ(0, memcmp(data.asioData(0,0), expectL, 40));
	ASSERT_EQ(0, memcmp(data.asioData(1,0), expectR, 40));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData, dsdLSBSampleToDSDInt8MSB1Stereo)
{
	const tubyte c_sampleInput[80] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, // 0 : L
		0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, // 0 : R
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, // 1 : L
		0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, // 1 : R
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, // 2 : L
		0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, // 2 : R
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, // 3 : L
		0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, // 3 : R
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, // 4 : L
		0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50  // 4 : R
	};
	
	const tubyte c_expectL[40] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, // 0 : L
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, // 1 : L
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, // 2 : L
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, // 3 : L
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, // 4 : L
	};
	
	const tubyte c_expectR[40] = {
		0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, // 0 : R
		0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, // 1 : R
		0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, // 2 : R
		0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, // 3 : R
		0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50  // 4 : R
	};
	
	tubyte expectL[40], expectR[40];
	for(tint i = 0; i < 40; i++)
	{
		expectL[i] = engine::lsb2msb(c_expectL[i]);
		expectR[i] = engine::lsb2msb(c_expectR[i]);
	}
	
	ASIOData data(5, 2, 2);
	data.setSampleType(ASIOSTDSDInt8MSB1);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0), c_sampleInput, 80);
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleDSD8LSB);
	
	data.convert();
	
	ASSERT_EQ(0, memcmp(data.asioData(0,0), expectL, 40));
	ASSERT_EQ(0, memcmp(data.asioData(1,0), expectR, 40));
}

//-------------------------------------------------------------------------------------------

TEST(ASIOData, dsdMSBSampleToDSDInt8MSB1Stereo)
{
	const tubyte c_sampleInput[80] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, // 0 : L
		0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, // 0 : R
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, // 1 : L
		0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, // 1 : R
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, // 2 : L
		0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, // 2 : R
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, // 3 : L
		0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, // 3 : R
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, // 4 : L
		0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50  // 4 : R
	};
	
	const tubyte c_expectL[40] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, // 0 : L
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, // 1 : L
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, // 2 : L
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, // 3 : L
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, // 4 : L
	};
	
	const tubyte c_expectR[40] = {
		0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, // 0 : R
		0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, // 1 : R
		0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, // 2 : R
		0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, // 3 : R
		0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50  // 4 : R
	};
	
	ASIOData data(5, 2, 2);
	data.setSampleType(ASIOSTDSDInt8MSB1);
	
	engine::RData::Part& part = data.nextPart();
	memcpy(data.partDataOut(0), c_sampleInput, 80);
	part.length() = 5;
	part.done() = true;
	part.setDataType(engine::e_SampleDSD8MSB);
	
	data.convert();
	
	ASSERT_EQ(0, memcmp(data.asioData(0,0), c_expectL, 40));
	ASSERT_EQ(0, memcmp(data.asioData(1,0), c_expectR, 40));
}

//-------------------------------------------------------------------------------------------
