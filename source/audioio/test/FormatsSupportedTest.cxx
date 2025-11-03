#include "gtest/gtest.h"

#include "audioio/inc/FormatsSupported.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

class FormatsSupportedTest : public FormatsSupported
{
    public:
        FormatsSupportedTest();

        tuint32 testToKey(const FormatDescription& desc);
        FormatDescription testFromKey(tuint32 key);
};

//-------------------------------------------------------------------------------------------

FormatsSupportedTest::FormatsSupportedTest() : FormatsSupported()
{}

//-------------------------------------------------------------------------------------------

tuint32 FormatsSupportedTest::testToKey(const FormatDescription& desc)
{
    return toKey(desc);
}

//-------------------------------------------------------------------------------------------

FormatDescription FormatsSupportedTest::testFromKey(tuint32 key)
{
    return fromKey(key);
}

//-------------------------------------------------------------------------------------------

TEST(FormatsSupported,toKey)
{
    FormatsSupportedTest support;

    FormatDescription formatA(FormatDescription::e_DataFloatSingle,32,1,8000); // 0, 4, 0, 0;
    // |xxxx|xxxx|xxxe|dddd|bbbb|bbbc|cccf|ffff|
    // |0000|0000|0000|0000|0000|1000|0000|0000| = 0x00000800
    EXPECT_EQ(0x00000800,support.testToKey(formatA));

    FormatDescription formatB(FormatDescription::e_DataUnsignedInteger,31,8,768000); // 3, 33, 7, 17
    // |xxxx|xxxx|xxxe|dddd|bbbb|bbbc|cccf|ffff|
    // |0000|0000|0000|0011|0100|0010|1111|0001| = 0x000342f1
    EXPECT_EQ(0x000342f1,support.testToKey(formatB));

    FormatDescription formatC; // 2, 1, 1, 7
    // |xxxx|xxxx|xxxe|dddd|bbbb|bbbc|cccf|ffff|
    // |0000|0000|0000|0010|0000|0010|0010|0111| = 0x00020227
    EXPECT_EQ(0x00020227,support.testToKey(formatC));

    FormatDescription formatD(FormatDescription::e_DataFloatSingle,32,1,8000,false); // 0, 4, 0, 0;
    // |xxxx|xxxx|xxxe|dddd|bbbb|bbbc|cccf|ffff|
    // |0000|0000|0001|0000|0000|1000|0000|0000| = 0x00100800
    EXPECT_EQ(0x00100800,support.testToKey(formatD));
}

//-------------------------------------------------------------------------------------------

TEST(FormatsSupported,fromKey)
{
    FormatsSupportedTest support;

    FormatDescription formatA(support.testFromKey(0x00000800));
    EXPECT_EQ(FormatDescription::e_DataFloatSingle,formatA.typeOfData());
    EXPECT_EQ(32,formatA.bits());
    EXPECT_EQ(1,formatA.channels());
    EXPECT_EQ(8000,formatA.frequency());
    EXPECT_TRUE(formatA.isLittleEndian());

    FormatDescription formatB(support.testFromKey(0x000342f1));
    EXPECT_EQ(FormatDescription::e_DataUnsignedInteger,formatB.typeOfData());
    EXPECT_EQ(31,formatB.bits());
    EXPECT_EQ(8,formatB.channels());
    EXPECT_EQ(768000,formatB.frequency());
    EXPECT_TRUE(formatB.isLittleEndian());

    FormatDescription formatC(support.testFromKey(0x00020227));
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatC.typeOfData());
    EXPECT_EQ(16,formatC.bits());
    EXPECT_EQ(2,formatC.channels());
    EXPECT_EQ(44100,formatC.frequency());
    EXPECT_TRUE(formatC.isLittleEndian());

    FormatDescription formatD(support.testFromKey(0x00100800));
    EXPECT_EQ(FormatDescription::e_DataFloatSingle,formatD.typeOfData());
    EXPECT_EQ(32,formatD.bits());
    EXPECT_EQ(1,formatD.channels());
    EXPECT_EQ(8000,formatD.frequency());
    EXPECT_FALSE(formatD.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(FormatsSupported,addAndIsSupported)
{
    // Supported
    FormatDescription formatA(FormatDescription::e_DataFloatSingle,16,2,44100);
    FormatDescription formatB(FormatDescription::e_DataFloatSingle,24,2,44100);
    FormatDescription formatC(FormatDescription::e_DataFloatSingle,24,2,48000);

    FormatDescription formatD(FormatDescription::e_DataSignedInteger,16,2,44100);
    FormatDescription formatE(FormatDescription::e_DataSignedInteger,16,2,48000);
    FormatDescription formatF(FormatDescription::e_DataSignedInteger,24,2,44100);
    FormatDescription formatG(FormatDescription::e_DataSignedInteger,24,2,48000);

    FormatDescription formatH(FormatDescription::e_DataSignedInteger,24,8,44100);
    FormatDescription formatI(FormatDescription::e_DataSignedInteger,24,8,48000);
    FormatDescription formatJ(FormatDescription::e_DataSignedInteger,24,4,192000);

    FormatDescription formatEndianA(FormatDescription::e_DataFloatSingle,24,2,96000,false);

    // Supported test formats
    FormatDescription testFormatA(FormatDescription::e_DataFloatSingle,16,2,44100);
    FormatDescription testFormatB(FormatDescription::e_DataFloatSingle,24,2,44100);
    FormatDescription testFormatC(FormatDescription::e_DataFloatSingle,24,2,48000);

    FormatDescription testFormatD(FormatDescription::e_DataSignedInteger,16,2,44100);
    FormatDescription testFormatE(FormatDescription::e_DataSignedInteger,16,2,48000);
    FormatDescription testFormatF(FormatDescription::e_DataSignedInteger,24,2,44100);
    FormatDescription testFormatG(FormatDescription::e_DataSignedInteger,24,2,48000);

    FormatDescription testFormatH(FormatDescription::e_DataSignedInteger,24,8,44100);
    FormatDescription testFormatI(FormatDescription::e_DataSignedInteger,24,8,48000);
    FormatDescription testFormatJ(FormatDescription::e_DataSignedInteger,24,4,192000);

    FormatDescription testFormatEndianA(FormatDescription::e_DataFloatSingle,24,2,96000,false);

    // Unsupported test formats
    FormatDescription testFormatK(FormatDescription::e_DataFloatSingle,32,2,44100);
    FormatDescription testFormatL(FormatDescription::e_DataFloatSingle,24,4,44100);
    FormatDescription testFormatM(FormatDescription::e_DataFloatSingle,24,2,96000);

    FormatDescription testFormatN(FormatDescription::e_DataUnsignedInteger,16,2,44100);
    FormatDescription testFormatO(FormatDescription::e_DataSignedInteger,8,2,48000);
    FormatDescription testFormatP(FormatDescription::e_DataSignedInteger,24,4,44100);
    FormatDescription testFormatQ(FormatDescription::e_DataSignedInteger,24,2,8000);

    FormatDescription testFormatR(FormatDescription::e_DataSignedInteger,24,8,88200);
    FormatDescription testFormatS(FormatDescription::e_DataSignedInteger,24,8,96000);
    FormatDescription testFormatT(FormatDescription::e_DataSignedInteger,24,4,176400);

    FormatDescription testFormatEndianB(FormatDescription::e_DataSignedInteger,24,4,192000,false);

    FormatsSupported support;
    support.add(formatA);
    support.add(formatB);
    support.add(formatC);
    support.add(formatD);
    support.add(formatE);
    support.add(formatF);
    support.add(formatG);
    support.add(formatH);
    support.add(formatI);
    support.add(formatJ);
    support.add(formatEndianA);

    EXPECT_TRUE(support.isSupported(testFormatA));
    EXPECT_TRUE(support.isSupported(testFormatB));
    EXPECT_TRUE(support.isSupported(testFormatC));
    EXPECT_TRUE(support.isSupported(testFormatD));
    EXPECT_TRUE(support.isSupported(testFormatE));
    EXPECT_TRUE(support.isSupported(testFormatF));
    EXPECT_TRUE(support.isSupported(testFormatG));
    EXPECT_TRUE(support.isSupported(testFormatH));
    EXPECT_TRUE(support.isSupported(testFormatI));
    EXPECT_TRUE(support.isSupported(testFormatJ));
    EXPECT_TRUE(support.isSupported(testFormatEndianA));

    EXPECT_FALSE(support.isSupported(testFormatK));
    EXPECT_FALSE(support.isSupported(testFormatL));
    EXPECT_FALSE(support.isSupported(testFormatM));
    EXPECT_FALSE(support.isSupported(testFormatN));
    EXPECT_FALSE(support.isSupported(testFormatO));
    EXPECT_FALSE(support.isSupported(testFormatP));
    EXPECT_FALSE(support.isSupported(testFormatQ));
    EXPECT_FALSE(support.isSupported(testFormatR));
    EXPECT_FALSE(support.isSupported(testFormatS));
    EXPECT_FALSE(support.isSupported(testFormatT));
    EXPECT_FALSE(support.isSupported(testFormatEndianB));
}

//-------------------------------------------------------------------------------------------

TEST(FormatsSupported,addAndIsSupportedWhenCopiedFromConstructor)
{
    // Supported
    FormatDescription formatA(FormatDescription::e_DataFloatSingle,16,2,44100);
    FormatDescription formatB(FormatDescription::e_DataFloatSingle,24,2,44100);
    FormatDescription formatC(FormatDescription::e_DataFloatSingle,24,2,48000);

    FormatDescription formatD(FormatDescription::e_DataSignedInteger,16,2,44100);
    FormatDescription formatE(FormatDescription::e_DataSignedInteger,16,2,48000);
    FormatDescription formatF(FormatDescription::e_DataSignedInteger,24,2,44100);
    FormatDescription formatG(FormatDescription::e_DataSignedInteger,24,2,48000);

    FormatDescription formatH(FormatDescription::e_DataSignedInteger,24,8,44100);
    FormatDescription formatI(FormatDescription::e_DataSignedInteger,24,8,48000);
    FormatDescription formatJ(FormatDescription::e_DataSignedInteger,24,4,192000);

    // Supported test formats
    FormatDescription testFormatA(FormatDescription::e_DataFloatSingle,16,2,44100);
    FormatDescription testFormatB(FormatDescription::e_DataFloatSingle,24,2,44100);
    FormatDescription testFormatC(FormatDescription::e_DataFloatSingle,24,2,48000);

    FormatDescription testFormatD(FormatDescription::e_DataSignedInteger,16,2,44100);
    FormatDescription testFormatE(FormatDescription::e_DataSignedInteger,16,2,48000);
    FormatDescription testFormatF(FormatDescription::e_DataSignedInteger,24,2,44100);
    FormatDescription testFormatG(FormatDescription::e_DataSignedInteger,24,2,48000);

    FormatDescription testFormatH(FormatDescription::e_DataSignedInteger,24,8,44100);
    FormatDescription testFormatI(FormatDescription::e_DataSignedInteger,24,8,48000);
    FormatDescription testFormatJ(FormatDescription::e_DataSignedInteger,24,4,192000);

    // Unsupported test formats
    FormatDescription testFormatK(FormatDescription::e_DataFloatSingle,32,2,44100);
    FormatDescription testFormatL(FormatDescription::e_DataFloatSingle,24,4,44100);
    FormatDescription testFormatM(FormatDescription::e_DataFloatSingle,24,2,96000);

    FormatDescription testFormatN(FormatDescription::e_DataUnsignedInteger,16,2,44100);
    FormatDescription testFormatO(FormatDescription::e_DataSignedInteger,8,2,48000);
    FormatDescription testFormatP(FormatDescription::e_DataSignedInteger,24,4,44100);
    FormatDescription testFormatQ(FormatDescription::e_DataSignedInteger,24,2,8000);

    FormatDescription testFormatR(FormatDescription::e_DataSignedInteger,24,8,88200);
    FormatDescription testFormatS(FormatDescription::e_DataSignedInteger,24,8,96000);
    FormatDescription testFormatT(FormatDescription::e_DataSignedInteger,24,4,176400);

    FormatsSupported supportA;
    supportA.add(formatA);
    supportA.add(formatB);
    supportA.add(formatC);
    supportA.add(formatD);
    supportA.add(formatE);
    supportA.add(formatF);
    supportA.add(formatG);
    supportA.add(formatH);
    supportA.add(formatI);
    supportA.add(formatJ);

    FormatsSupported support(supportA);

    EXPECT_TRUE(support.isSupported(testFormatA));
    EXPECT_TRUE(support.isSupported(testFormatB));
    EXPECT_TRUE(support.isSupported(testFormatC));
    EXPECT_TRUE(support.isSupported(testFormatD));
    EXPECT_TRUE(support.isSupported(testFormatE));
    EXPECT_TRUE(support.isSupported(testFormatF));
    EXPECT_TRUE(support.isSupported(testFormatG));
    EXPECT_TRUE(support.isSupported(testFormatH));
    EXPECT_TRUE(support.isSupported(testFormatI));
    EXPECT_TRUE(support.isSupported(testFormatJ));

    EXPECT_FALSE(support.isSupported(testFormatK));
    EXPECT_FALSE(support.isSupported(testFormatL));
    EXPECT_FALSE(support.isSupported(testFormatM));
    EXPECT_FALSE(support.isSupported(testFormatN));
    EXPECT_FALSE(support.isSupported(testFormatO));
    EXPECT_FALSE(support.isSupported(testFormatP));
    EXPECT_FALSE(support.isSupported(testFormatQ));
    EXPECT_FALSE(support.isSupported(testFormatR));
    EXPECT_FALSE(support.isSupported(testFormatS));
    EXPECT_FALSE(support.isSupported(testFormatT));
}

//-------------------------------------------------------------------------------------------

TEST(FormatsSupported,addAndIsSupportedWhenCopiedViaEqualsOperator)
{
    // Supported
    FormatDescription formatA(FormatDescription::e_DataFloatSingle,16,2,44100);
    FormatDescription formatB(FormatDescription::e_DataFloatSingle,24,2,44100);
    FormatDescription formatC(FormatDescription::e_DataFloatSingle,24,2,48000);

    FormatDescription formatD(FormatDescription::e_DataSignedInteger,16,2,44100);
    FormatDescription formatE(FormatDescription::e_DataSignedInteger,16,2,48000);
    FormatDescription formatF(FormatDescription::e_DataSignedInteger,24,2,44100);
    FormatDescription formatG(FormatDescription::e_DataSignedInteger,24,2,48000);

    FormatDescription formatH(FormatDescription::e_DataSignedInteger,24,8,44100);
    FormatDescription formatI(FormatDescription::e_DataSignedInteger,24,8,48000);
    FormatDescription formatJ(FormatDescription::e_DataSignedInteger,24,4,192000);

    // Supported test formats
    FormatDescription testFormatA(FormatDescription::e_DataFloatSingle,16,2,44100);
    FormatDescription testFormatB(FormatDescription::e_DataFloatSingle,24,2,44100);
    FormatDescription testFormatC(FormatDescription::e_DataFloatSingle,24,2,48000);

    FormatDescription testFormatD(FormatDescription::e_DataSignedInteger,16,2,44100);
    FormatDescription testFormatE(FormatDescription::e_DataSignedInteger,16,2,48000);
    FormatDescription testFormatF(FormatDescription::e_DataSignedInteger,24,2,44100);
    FormatDescription testFormatG(FormatDescription::e_DataSignedInteger,24,2,48000);

    FormatDescription testFormatH(FormatDescription::e_DataSignedInteger,24,8,44100);
    FormatDescription testFormatI(FormatDescription::e_DataSignedInteger,24,8,48000);
    FormatDescription testFormatJ(FormatDescription::e_DataSignedInteger,24,4,192000);

    // Unsupported test formats
    FormatDescription testFormatK(FormatDescription::e_DataFloatSingle,32,2,44100);
    FormatDescription testFormatL(FormatDescription::e_DataFloatSingle,24,4,44100);
    FormatDescription testFormatM(FormatDescription::e_DataFloatSingle,24,2,96000);

    FormatDescription testFormatN(FormatDescription::e_DataUnsignedInteger,16,2,44100);
    FormatDescription testFormatO(FormatDescription::e_DataSignedInteger,8,2,48000);
    FormatDescription testFormatP(FormatDescription::e_DataSignedInteger,24,4,44100);
    FormatDescription testFormatQ(FormatDescription::e_DataSignedInteger,24,2,8000);

    FormatDescription testFormatR(FormatDescription::e_DataSignedInteger,24,8,88200);
    FormatDescription testFormatS(FormatDescription::e_DataSignedInteger,24,8,96000);
    FormatDescription testFormatT(FormatDescription::e_DataSignedInteger,24,4,176400);

    FormatsSupported supportA;
    supportA.add(formatA);
    supportA.add(formatB);
    supportA.add(formatC);
    supportA.add(formatD);
    supportA.add(formatE);
    supportA.add(formatF);
    supportA.add(formatG);
    supportA.add(formatH);
    supportA.add(formatI);
    supportA.add(formatJ);

    FormatsSupported support;
    support = supportA;

    EXPECT_TRUE(support.isSupported(testFormatA));
    EXPECT_TRUE(support.isSupported(testFormatB));
    EXPECT_TRUE(support.isSupported(testFormatC));
    EXPECT_TRUE(support.isSupported(testFormatD));
    EXPECT_TRUE(support.isSupported(testFormatE));
    EXPECT_TRUE(support.isSupported(testFormatF));
    EXPECT_TRUE(support.isSupported(testFormatG));
    EXPECT_TRUE(support.isSupported(testFormatH));
    EXPECT_TRUE(support.isSupported(testFormatI));
    EXPECT_TRUE(support.isSupported(testFormatJ));

    EXPECT_FALSE(support.isSupported(testFormatK));
    EXPECT_FALSE(support.isSupported(testFormatL));
    EXPECT_FALSE(support.isSupported(testFormatM));
    EXPECT_FALSE(support.isSupported(testFormatN));
    EXPECT_FALSE(support.isSupported(testFormatO));
    EXPECT_FALSE(support.isSupported(testFormatP));
    EXPECT_FALSE(support.isSupported(testFormatQ));
    EXPECT_FALSE(support.isSupported(testFormatR));
    EXPECT_FALSE(support.isSupported(testFormatS));
    EXPECT_FALSE(support.isSupported(testFormatT));
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescription,setOfFrequencies)
{
    QSet<tint> freqSet = FormatDescription::setOfFrequencies();
    ASSERT_EQ(18,freqSet.size());
    EXPECT_TRUE(freqSet.contains(8000));
    EXPECT_TRUE(freqSet.contains(11025));
    EXPECT_TRUE(freqSet.contains(12000));
    EXPECT_TRUE(freqSet.contains(16000));
    EXPECT_TRUE(freqSet.contains(22050));
    EXPECT_TRUE(freqSet.contains(24000));
    EXPECT_TRUE(freqSet.contains(32000));
    EXPECT_TRUE(freqSet.contains(44100));
    EXPECT_TRUE(freqSet.contains(48000));
    EXPECT_TRUE(freqSet.contains(64000));
    EXPECT_TRUE(freqSet.contains(88200));
    EXPECT_TRUE(freqSet.contains(96000));
    EXPECT_TRUE(freqSet.contains(176400));
    EXPECT_TRUE(freqSet.contains(192000));
    EXPECT_TRUE(freqSet.contains(352800));
    EXPECT_TRUE(freqSet.contains(384000));
    EXPECT_TRUE(freqSet.contains(705600));
    EXPECT_TRUE(freqSet.contains(768000));
}

//-------------------------------------------------------------------------------------------
