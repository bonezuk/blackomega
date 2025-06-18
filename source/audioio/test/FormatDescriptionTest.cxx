#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/FormatDescription.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,constructorDefault)
{
    FormatDescription format;

    EXPECT_EQ(FormatDescription::e_DataSignedInteger,format.typeOfData());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(1,format.bitsIndex());

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(1,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(7,format.frequencyIndex());

    EXPECT_TRUE(format.isLittleEndian());
    EXPECT_FALSE(format.isBigEndian());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,constructorSetFormatValid)
{
    FormatDescription format(FormatDescription::e_DataUnsignedInteger,24,4,96000);

    EXPECT_EQ(FormatDescription::e_DataUnsignedInteger,format.typeOfData());

    EXPECT_EQ(24,format.bits());
    EXPECT_EQ(2,format.bitsIndex());

    EXPECT_EQ(4,format.channels());
    EXPECT_EQ(3,format.channelsIndex());

    EXPECT_EQ(96000,format.frequency());
    EXPECT_EQ(11,format.frequencyIndex());

    EXPECT_TRUE(format.isLittleEndian());
    EXPECT_FALSE(format.isBigEndian());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,constructorSetFormatInvalidBits)
{
    FormatDescription format(FormatDescription::e_DataUnsignedInteger,62,4,96000);

    EXPECT_EQ(FormatDescription::e_DataUnsignedInteger,format.typeOfData());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(1,format.bitsIndex());

    EXPECT_EQ(4,format.channels());
    EXPECT_EQ(3,format.channelsIndex());

    EXPECT_EQ(96000,format.frequency());
    EXPECT_EQ(11,format.frequencyIndex());

    EXPECT_TRUE(format.isLittleEndian());
    EXPECT_FALSE(format.isBigEndian());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,constructorSetFormatInvalidChannels)
{
    FormatDescription format(FormatDescription::e_DataUnsignedInteger,24,9,96000);

    EXPECT_EQ(FormatDescription::e_DataUnsignedInteger,format.typeOfData());

    EXPECT_EQ(24,format.bits());
    EXPECT_EQ(2,format.bitsIndex());

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(1,format.channelsIndex());

    EXPECT_EQ(96000,format.frequency());
    EXPECT_EQ(11,format.frequencyIndex());

    EXPECT_TRUE(format.isLittleEndian());
    EXPECT_FALSE(format.isBigEndian());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,constructorSetFormatInvalidFrequency)
{
    FormatDescription format(FormatDescription::e_DataUnsignedInteger,24,4,1000);

    EXPECT_EQ(FormatDescription::e_DataUnsignedInteger,format.typeOfData());

    EXPECT_EQ(24,format.bits());
    EXPECT_EQ(2,format.bitsIndex());

    EXPECT_EQ(4,format.channels());
    EXPECT_EQ(3,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(7,format.frequencyIndex());

    EXPECT_TRUE(format.isLittleEndian());
    EXPECT_FALSE(format.isBigEndian());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,constructorSetFormatValidWithBigEndian)
{
    FormatDescription format(FormatDescription::e_DataUnsignedInteger,24,4,96000,false);

    EXPECT_EQ(FormatDescription::e_DataUnsignedInteger,format.typeOfData());

    EXPECT_EQ(24,format.bits());
    EXPECT_EQ(2,format.bitsIndex());

    EXPECT_EQ(4,format.channels());
    EXPECT_EQ(3,format.channelsIndex());

    EXPECT_EQ(96000,format.frequency());
    EXPECT_EQ(11,format.frequencyIndex());

    EXPECT_FALSE(format.isLittleEndian());
    EXPECT_TRUE(format.isBigEndian());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,constructorCopy)
{
    FormatDescription formatB(FormatDescription::e_DataUnsignedInteger,24,4,96000,false);
    FormatDescription format(formatB);

    EXPECT_EQ(FormatDescription::e_DataUnsignedInteger,format.typeOfData());

    EXPECT_EQ(24,format.bits());
    EXPECT_EQ(2,format.bitsIndex());

    EXPECT_EQ(4,format.channels());
    EXPECT_EQ(3,format.channelsIndex());

    EXPECT_EQ(96000,format.frequency());
    EXPECT_EQ(11,format.frequencyIndex());

    EXPECT_FALSE(format.isLittleEndian());
    EXPECT_TRUE(format.isBigEndian());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,equalOperator)
{
    FormatDescription formatB(FormatDescription::e_DataUnsignedInteger,24,4,96000,false);
    FormatDescription format;

    format = formatB;

    EXPECT_EQ(FormatDescription::e_DataUnsignedInteger,format.typeOfData());

    EXPECT_EQ(24,format.bits());
    EXPECT_EQ(2,format.bitsIndex());

    EXPECT_EQ(4,format.channels());
    EXPECT_EQ(3,format.channelsIndex());

    EXPECT_EQ(96000,format.frequency());
    EXPECT_EQ(11,format.frequencyIndex());

    EXPECT_FALSE(format.isLittleEndian());
    EXPECT_TRUE(format.isBigEndian());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,isEqualOperator)
{
    FormatDescription format(FormatDescription::e_DataUnsignedInteger,24,4,96000);

    FormatDescription cmpFormatA(FormatDescription::e_DataFloatSingle,24,4,96000);
    EXPECT_FALSE(format == cmpFormatA);

    FormatDescription cmpFormatB(FormatDescription::e_DataUnsignedInteger,16,4,96000);
    EXPECT_FALSE(format == cmpFormatB);

    FormatDescription cmpFormatC(FormatDescription::e_DataUnsignedInteger,24,2,96000);
    EXPECT_FALSE(format == cmpFormatC);

    FormatDescription cmpFormatD(FormatDescription::e_DataUnsignedInteger,24,4,48000);
    EXPECT_FALSE(format == cmpFormatD);

    FormatDescription cmpFormatE(FormatDescription::e_DataUnsignedInteger,24,4,96000);
    EXPECT_TRUE(format == cmpFormatE);

    FormatDescription cmpFormatF(FormatDescription::e_DataUnsignedInteger,24,4,96000,false);
    EXPECT_FALSE(format == cmpFormatF);
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,isNotEqualOperator)
{
    FormatDescription format(FormatDescription::e_DataUnsignedInteger,24,4,96000);

    FormatDescription cmpFormatA(FormatDescription::e_DataFloatSingle,24,4,96000);
    EXPECT_TRUE(format != cmpFormatA);

    FormatDescription cmpFormatB(FormatDescription::e_DataUnsignedInteger,16,4,96000);
    EXPECT_TRUE(format != cmpFormatB);

    FormatDescription cmpFormatC(FormatDescription::e_DataUnsignedInteger,24,2,96000);
    EXPECT_TRUE(format != cmpFormatC);

    FormatDescription cmpFormatD(FormatDescription::e_DataUnsignedInteger,24,4,48000);
    EXPECT_TRUE(format != cmpFormatD);

    FormatDescription cmpFormatE(FormatDescription::e_DataUnsignedInteger,24,4,96000);
    EXPECT_FALSE(format != cmpFormatE);

    FormatDescription cmpFormatF(FormatDescription::e_DataUnsignedInteger,24,4,96000,false);
    EXPECT_TRUE(format != cmpFormatF);
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,bitsIndexWhenInteger8Bits)
{
    FormatDescription format;
    ASSERT_TRUE(format.setNumberOfBits(8));

    EXPECT_EQ(8,format.bits());
    EXPECT_EQ(0,format.bitsIndex());
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,format.typeOfData());

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setBitsIndexForInteger8Bits)
{
    FormatDescription format;
    format.setTypeOfData(FormatDescription::e_DataFloatSingle);
    ASSERT_TRUE(format.setBitsIndex(0));

    EXPECT_EQ(8,format.bits());
    EXPECT_EQ(0,format.bitsIndex());
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,format.typeOfData());

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,bitsIndexWhenInteger16Bits)
{
    FormatDescription format;
    ASSERT_TRUE(format.setNumberOfBits(16));

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(1,format.bitsIndex());
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,format.typeOfData());

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setBitsIndexForInteger16Bits)
{
    FormatDescription format;
    format.setTypeOfData(FormatDescription::e_DataFloatSingle);

    ASSERT_TRUE(format.setBitsIndex(1));

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(1,format.bitsIndex());
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,format.typeOfData());

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,bitsIndexWhenInteger24Bits)
{
    FormatDescription format;
    ASSERT_TRUE(format.setNumberOfBits(24));

    EXPECT_EQ(24,format.bits());
    EXPECT_EQ(2,format.bitsIndex());
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,format.typeOfData());

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setBitsIndexForInteger24Bits)
{
    FormatDescription format;
    format.setTypeOfData(FormatDescription::e_DataFloatSingle);
    ASSERT_TRUE(format.setBitsIndex(2));

    EXPECT_EQ(24,format.bits());
    EXPECT_EQ(2,format.bitsIndex());
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,format.typeOfData());

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,bitsIndexWhenInteger32Bits)
{
    FormatDescription format;
    ASSERT_TRUE(format.setNumberOfBits(32));

    EXPECT_EQ(32,format.bits());
    EXPECT_EQ(3,format.bitsIndex());
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,format.typeOfData());

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setBitsIndexForInteger32Bits)
{
    FormatDescription format;
    format.setTypeOfData(FormatDescription::e_DataFloatSingle);
    ASSERT_TRUE(format.setBitsIndex(3));

    EXPECT_EQ(32,format.bits());
    EXPECT_EQ(3,format.bitsIndex());
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,format.typeOfData());

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,bitsIndexWhenFloatSingle)
{
    FormatDescription format;
    ASSERT_TRUE(format.setTypeOfData(FormatDescription::e_DataFloatSingle));

    EXPECT_EQ(32,format.bits());
    EXPECT_EQ(4,format.bitsIndex());
    EXPECT_EQ(FormatDescription::e_DataFloatSingle,format.typeOfData());

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setBitsIndexForFloatSingle)
{
    FormatDescription format;
    ASSERT_TRUE(format.setBitsIndex(4));

    EXPECT_EQ(32,format.bits());
    EXPECT_EQ(4,format.bitsIndex());
    EXPECT_EQ(FormatDescription::e_DataFloatSingle,format.typeOfData());

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,bitsIndexWhenFloatDouble)
{
    FormatDescription format;
    ASSERT_TRUE(format.setTypeOfData(FormatDescription::e_DataFloatDouble));

    EXPECT_EQ(64,format.bits());
    EXPECT_EQ(5,format.bitsIndex());
    EXPECT_EQ(FormatDescription::e_DataFloatDouble,format.typeOfData());

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,bitsIndexWhen64BitsSet)
{
    FormatDescription format;
    ASSERT_TRUE(format.setNumberOfBits(64));

    EXPECT_EQ(64,format.bits());
    EXPECT_EQ(5,format.bitsIndex());
    EXPECT_EQ(FormatDescription::e_DataFloatDouble,format.typeOfData());

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setBitsIndexForFloatDouble)
{
    FormatDescription format;
    ASSERT_TRUE(format.setBitsIndex(5));

    EXPECT_EQ(64,format.bits());
    EXPECT_EQ(5,format.bitsIndex());
    EXPECT_EQ(FormatDescription::e_DataFloatDouble,format.typeOfData());

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setNumberOfBitsWhenValidAndSignedInteger)
{
    const tint bitsArray[28] = {
         1,  2,  3,  4,  5,  6,  7,
         9, 10, 11, 12, 13, 14, 15,
        17, 18, 19, 20, 21, 22, 23,
        25, 26, 27, 28, 29, 30, 31
    };

    const tint indexArray[28] = {
         6,  7,  8,  9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26,
        27, 28, 29, 30, 31, 32, 33
    };

    FormatDescription format;
    ASSERT_TRUE(format.setTypeOfData(FormatDescription::e_DataSignedInteger));

    for(tint i=0;i<28;i++)
    {
        ASSERT_TRUE(format.setNumberOfBits(bitsArray[i]));
        EXPECT_EQ(bitsArray[i],format.bits());
        EXPECT_EQ(indexArray[i],format.bitsIndex());
        EXPECT_EQ(FormatDescription::e_DataSignedInteger,format.typeOfData());
    }

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setBitsIndexWhenValidAndSignedInteger)
{
    const tint bitsArray[28] = {
         1,  2,  3,  4,  5,  6,  7,
         9, 10, 11, 12, 13, 14, 15,
        17, 18, 19, 20, 21, 22, 23,
        25, 26, 27, 28, 29, 30, 31
    };

    const tint indexArray[28] = {
         6,  7,  8,  9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26,
        27, 28, 29, 30, 31, 32, 33
    };

    FormatDescription format;
    ASSERT_TRUE(format.setTypeOfData(FormatDescription::e_DataSignedInteger));

    for(tint i=0;i<28;i++)
    {
        ASSERT_TRUE(format.setBitsIndex(indexArray[i]));
        EXPECT_EQ(bitsArray[i],format.bits());
        EXPECT_EQ(indexArray[i],format.bitsIndex());
        EXPECT_EQ(FormatDescription::e_DataSignedInteger,format.typeOfData());
    }

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setNumberOfBitsWhenValidAndUnsignedInteger)
{
    const tint bitsArray[28] = {
         1,  2,  3,  4,  5,  6,  7,
         9, 10, 11, 12, 13, 14, 15,
        17, 18, 19, 20, 21, 22, 23,
        25, 26, 27, 28, 29, 30, 31
    };

    const tint indexArray[28] = {
         6,  7,  8,  9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26,
        27, 28, 29, 30, 31, 32, 33
    };

    FormatDescription format;
    format.setTypeOfData(FormatDescription::e_DataUnsignedInteger);

    for(tint i=0;i<28;i++)
    {
        ASSERT_TRUE(format.setNumberOfBits(bitsArray[i]));
        EXPECT_EQ(bitsArray[i],format.bits());
        EXPECT_EQ(indexArray[i],format.bitsIndex());
        EXPECT_EQ(FormatDescription::e_DataUnsignedInteger,format.typeOfData());
    }

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setBitsIndexWhenValidAndUnsignedInteger)
{
    const tint bitsArray[28] = {
         1,  2,  3,  4,  5,  6,  7,
         9, 10, 11, 12, 13, 14, 15,
        17, 18, 19, 20, 21, 22, 23,
        25, 26, 27, 28, 29, 30, 31
    };

    const tint indexArray[28] = {
         6,  7,  8,  9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26,
        27, 28, 29, 30, 31, 32, 33
    };

    FormatDescription format;
    format.setTypeOfData(FormatDescription::e_DataUnsignedInteger);

    for(tint i=0;i<28;i++)
    {
        ASSERT_TRUE(format.setBitsIndex(indexArray[i]));
        EXPECT_EQ(bitsArray[i],format.bits());
        EXPECT_EQ(indexArray[i],format.bitsIndex());
        EXPECT_EQ(FormatDescription::e_DataUnsignedInteger,format.typeOfData());
    }

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setNumberOfBitsWhenInvalid)
{
    FormatDescription format;

    ASSERT_FALSE(format.setNumberOfBits(-1));
    ASSERT_FALSE(format.setNumberOfBits(33));

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setBitsIndexWhenInvalid)
{
    FormatDescription format;

    ASSERT_FALSE(format.setBitsIndex(-1));
    ASSERT_FALSE(format.setBitsIndex(34));

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(44100,format.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt8000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(8000));

    EXPECT_EQ(8000,format.frequency());
    EXPECT_EQ(0,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt8000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(0));

    EXPECT_EQ(8000,format.frequency());
    EXPECT_EQ(0,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt11025Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(11025));

    EXPECT_EQ(11025,format.frequency());
    EXPECT_EQ(1,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt11025Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(1));

    EXPECT_EQ(11025,format.frequency());
    EXPECT_EQ(1,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt12000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(12000));

    EXPECT_EQ(12000,format.frequency());
    EXPECT_EQ(2,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt12000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(2));

    EXPECT_EQ(12000,format.frequency());
    EXPECT_EQ(2,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt16000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(16000));

    EXPECT_EQ(16000,format.frequency());
    EXPECT_EQ(3,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt16000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(3));

    EXPECT_EQ(16000,format.frequency());
    EXPECT_EQ(3,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt22050Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(22050));

    EXPECT_EQ(22050,format.frequency());
    EXPECT_EQ(4,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt22050Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(4));

    EXPECT_EQ(22050,format.frequency());
    EXPECT_EQ(4,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt24000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(24000));

    EXPECT_EQ(24000,format.frequency());
    EXPECT_EQ(5,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt24000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(5));

    EXPECT_EQ(24000,format.frequency());
    EXPECT_EQ(5,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt32000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(32000));

    EXPECT_EQ(32000,format.frequency());
    EXPECT_EQ(6,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt32000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(6));

    EXPECT_EQ(32000,format.frequency());
    EXPECT_EQ(6,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt44100Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(44100));

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(7,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt44100Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(7));

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(7,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt48000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(48000));

    EXPECT_EQ(48000,format.frequency());
    EXPECT_EQ(8,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt48000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(8));

    EXPECT_EQ(48000,format.frequency());
    EXPECT_EQ(8,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt64000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(64000));

    EXPECT_EQ(64000,format.frequency());
    EXPECT_EQ(9,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt64000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(9));

    EXPECT_EQ(64000,format.frequency());
    EXPECT_EQ(9,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt88200Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(88200));

    EXPECT_EQ(88200,format.frequency());
    EXPECT_EQ(10,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt88200Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(10));

    EXPECT_EQ(88200,format.frequency());
    EXPECT_EQ(10,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt96000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(96000));

    EXPECT_EQ(96000,format.frequency());
    EXPECT_EQ(11,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt96000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(11));

    EXPECT_EQ(96000,format.frequency());
    EXPECT_EQ(11,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt176400Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(176400));

    EXPECT_EQ(176400,format.frequency());
    EXPECT_EQ(12,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt176400Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(12));

    EXPECT_EQ(176400,format.frequency());
    EXPECT_EQ(12,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt192000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(192000));

    EXPECT_EQ(192000,format.frequency());
    EXPECT_EQ(13,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt192000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(13));

    EXPECT_EQ(192000,format.frequency());
    EXPECT_EQ(13,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt352800Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(352800));

    EXPECT_EQ(352800,format.frequency());
    EXPECT_EQ(14,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt352800Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(14));

    EXPECT_EQ(352800,format.frequency());
    EXPECT_EQ(14,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt384000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(384000));

    EXPECT_EQ(384000,format.frequency());
    EXPECT_EQ(15,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt384000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(15));

    EXPECT_EQ(384000,format.frequency());
    EXPECT_EQ(15,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt705600Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(705600));

    EXPECT_EQ(705600,format.frequency());
    EXPECT_EQ(16,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt705600Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(16));

    EXPECT_EQ(705600,format.frequency());
    EXPECT_EQ(16,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyAt768000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequency(768000));

    EXPECT_EQ(768000,format.frequency());
    EXPECT_EQ(17,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexAt768000Hz)
{
    FormatDescription format;
    ASSERT_TRUE(format.setFrequencyIndex(17));

    EXPECT_EQ(768000,format.frequency());
    EXPECT_EQ(17,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyWhenInvalid)
{
    FormatDescription format;
    ASSERT_FALSE(format.setFrequency(1000));

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(7,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setFrequencyIndexWhenInvalid)
{
    FormatDescription format;
    ASSERT_FALSE(format.setFrequency(-1));
    ASSERT_FALSE(format.setFrequency(18));

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(7,format.frequencyIndex());

    EXPECT_EQ(16,format.bits());
    EXPECT_EQ(2,format.channels());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setNumberOfChannelsToMono)
{
    FormatDescription format;
    ASSERT_TRUE(format.setNumberOfChannels(1));

    EXPECT_EQ(1,format.channels());
    EXPECT_EQ(0,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setChannelsIndexForMono)
{
    FormatDescription format;
    ASSERT_TRUE(format.setChannelsIndex(0));

    EXPECT_EQ(1,format.channels());
    EXPECT_EQ(0,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setNumberOfChannelsToStereo)
{
    FormatDescription format;
    ASSERT_TRUE(format.setNumberOfChannels(2));

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(1,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setChannelsIndexForStereo)
{
    FormatDescription format;
    ASSERT_TRUE(format.setChannelsIndex(1));

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(1,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setNumberOfChannelsTo3Channels)
{
    FormatDescription format;
    ASSERT_TRUE(format.setNumberOfChannels(3));

    EXPECT_EQ(3,format.channels());
    EXPECT_EQ(2,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setChannelsIndexFor3Channels)
{
    FormatDescription format;
    ASSERT_TRUE(format.setChannelsIndex(2));

    EXPECT_EQ(3,format.channels());
    EXPECT_EQ(2,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setNumberOfChannelsTo4Channels)
{
    FormatDescription format;
    ASSERT_TRUE(format.setNumberOfChannels(4));

    EXPECT_EQ(4,format.channels());
    EXPECT_EQ(3,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setChannelsIndexFor4Channels)
{
    FormatDescription format;
    ASSERT_TRUE(format.setChannelsIndex(3));

    EXPECT_EQ(4,format.channels());
    EXPECT_EQ(3,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setNumberOfChannelsTo5Channels)
{
    FormatDescription format;
    ASSERT_TRUE(format.setNumberOfChannels(5));

    EXPECT_EQ(5,format.channels());
    EXPECT_EQ(4,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setChannelsIndexFor5Channels)
{
    FormatDescription format;
    ASSERT_TRUE(format.setChannelsIndex(4));

    EXPECT_EQ(5,format.channels());
    EXPECT_EQ(4,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setNumberOfChannelsTo6Channels)
{
    FormatDescription format;
    ASSERT_TRUE(format.setNumberOfChannels(6));

    EXPECT_EQ(6,format.channels());
    EXPECT_EQ(5,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setChannelsIndexFor6Channels)
{
    FormatDescription format;
    ASSERT_TRUE(format.setChannelsIndex(5));

    EXPECT_EQ(6,format.channels());
    EXPECT_EQ(5,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setNumberOfChannelsTo7Channels)
{
    FormatDescription format;
    ASSERT_TRUE(format.setNumberOfChannels(7));

    EXPECT_EQ(7,format.channels());
    EXPECT_EQ(6,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setChannelsIndexFor7Channels)
{
    FormatDescription format;
    ASSERT_TRUE(format.setChannelsIndex(6));

    EXPECT_EQ(7,format.channels());
    EXPECT_EQ(6,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setNumberOfChannelsTo8Channels)
{
    FormatDescription format;
    ASSERT_TRUE(format.setNumberOfChannels(8));

    EXPECT_EQ(8,format.channels());
    EXPECT_EQ(7,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setChannelsIndexFor8Channels)
{
    FormatDescription format;
    ASSERT_TRUE(format.setChannelsIndex(7));

    EXPECT_EQ(8,format.channels());
    EXPECT_EQ(7,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setNumberOfChannelsToInvalid)
{
    FormatDescription format;
    ASSERT_FALSE(format.setNumberOfChannels(0));
    ASSERT_FALSE(format.setNumberOfChannels(9));

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(1,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setChannelsIndexForInvalid)
{
    FormatDescription format;
    ASSERT_FALSE(format.setChannelsIndex(-1));
    ASSERT_FALSE(format.setChannelsIndex(8));

    EXPECT_EQ(2,format.channels());
    EXPECT_EQ(1,format.channelsIndex());

    EXPECT_EQ(44100,format.frequency());
    EXPECT_EQ(16,format.bits());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setEndian)
{
    FormatDescription format;

    format.setEndian(false);
    EXPECT_FALSE(format.isLittleEndian());
    EXPECT_TRUE(format.isBigEndian());

    format.setEndian(true);
    EXPECT_TRUE(format.isLittleEndian());
    EXPECT_FALSE(format.isBigEndian());
}

//-------------------------------------------------------------------------------------------
