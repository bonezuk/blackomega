#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/AOQueryALSA.h"
#include "audioio/test/LinuxALSAMockIF.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

class AOQueryALSAListOfCardsTest : public AOQueryALSA
{
    public:
        MOCK_CONST_METHOD3(printError,void(const tchar *strR,const tchar *strE,int rc));
        virtual QVector<tint> testListOfCards() const;
};

//-------------------------------------------------------------------------------------------

QVector<tint> AOQueryALSAListOfCardsTest::testListOfCards() const
{
    return listOfCards();
}

//-------------------------------------------------------------------------------------------

MATCHER_P(ALSASndCardNext,value,"") { return (*((int *)(arg)) == value); }
ACTION_P(SetALSASndCardNextForArgument,value) { *static_cast<int *>(arg0) = value; }

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSA,listOfCardsGivenErrorOnFirstCard)
{
    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_card_next(ALSASndCardNext(-1))).Times(1).WillOnce(DoAll(SetALSASndCardNextForArgument(5),Return(-3)));

    AOQueryALSAListOfCardsTest query;
    EXPECT_CALL(query,printError(StrEq("listOfCards"),StrEq("Failed to query next sound card"),-3)).Times(1);

    QVector<tint> cards = query.testListOfCards();

    ASSERT_EQ(0,cards.size());

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSA,listOfCardsGivenNoCards)
{
    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_card_next(ALSASndCardNext(-1))).Times(1).WillOnce(DoAll(SetALSASndCardNextForArgument(-1),Return(0)));

    AOQueryALSAListOfCardsTest query;

    QVector<tint> cards = query.testListOfCards();

    ASSERT_EQ(0,cards.size());

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSA,listOfCardsGivenOnlyOneCard)
{
    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    using ::testing::InSequence;
    {
        InSequence dummy;
        EXPECT_CALL(apiMock,snd_card_next(ALSASndCardNext(-1))).Times(1).WillOnce(DoAll(SetALSASndCardNextForArgument(0),Return(0)));
        EXPECT_CALL(apiMock,snd_card_next(ALSASndCardNext(0))).Times(1).WillOnce(DoAll(SetALSASndCardNextForArgument(-1),Return(0)));
    }

    AOQueryALSAListOfCardsTest query;

    QVector<tint> cards = query.testListOfCards();

    ASSERT_EQ(1,cards.size());
    ASSERT_EQ(0,cards.at(0));

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSA,listOfCardsGivenErrorOnQueryOfSecond)
{
    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    using ::testing::InSequence;
    {
        InSequence dummy;
        EXPECT_CALL(apiMock,snd_card_next(ALSASndCardNext(-1))).Times(1).WillOnce(DoAll(SetALSASndCardNextForArgument(6),Return(0)));
        EXPECT_CALL(apiMock,snd_card_next(ALSASndCardNext(6))).Times(1).WillOnce(DoAll(SetALSASndCardNextForArgument(5),Return(-2)));
    }

    AOQueryALSAListOfCardsTest query;
    EXPECT_CALL(query,printError(StrEq("listOfCards"),StrEq("Failed to query next sound card"),-2)).Times(1);

    QVector<tint> cards = query.testListOfCards();

    ASSERT_EQ(1,cards.size());
    ASSERT_EQ(6,cards.at(0));

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSA,listOfCardsGivenThreeCards)
{
    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    using ::testing::InSequence;
    {
        InSequence dummy;
        EXPECT_CALL(apiMock,snd_card_next(ALSASndCardNext(-1))).Times(1).WillOnce(DoAll(SetALSASndCardNextForArgument(0),Return(0)));
        EXPECT_CALL(apiMock,snd_card_next(ALSASndCardNext(0))).Times(1).WillOnce(DoAll(SetALSASndCardNextForArgument(5),Return(0)));
        EXPECT_CALL(apiMock,snd_card_next(ALSASndCardNext(5))).Times(1).WillOnce(DoAll(SetALSASndCardNextForArgument(8),Return(0)));
        EXPECT_CALL(apiMock,snd_card_next(ALSASndCardNext(8))).Times(1).WillOnce(DoAll(SetALSASndCardNextForArgument(-1),Return(0)));
    }

    AOQueryALSAListOfCardsTest query;

    QVector<tint> cards = query.testListOfCards();

    ASSERT_EQ(3,cards.size());
    ASSERT_EQ(0,cards.at(0));
    ASSERT_EQ(5,cards.at(1));
    ASSERT_EQ(8,cards.at(2));

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

class AOQueryALSADeviceALSAFormatFromDescriptionTest : public AOQueryALSA::DeviceALSA
{
    public:
        QVector<int> formatFromDescriptionTest(const FormatDescription& desc);
};

//-------------------------------------------------------------------------------------------

QVector<int> AOQueryALSADeviceALSAFormatFromDescriptionTest::formatFromDescriptionTest(const FormatDescription& desc)
{
    return formatsFromDescription(desc);
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForSigned8Bit)
{
    FormatDescription formatA(FormatDescription::e_DataSignedInteger,8,2,44100,true);
    FormatDescription formatB(FormatDescription::e_DataSignedInteger,8,2,44100,false);

    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    QVector<int> fA = device.formatFromDescriptionTest(formatA);
    ASSERT_EQ(1,fA.size());
    EXPECT_EQ(SND_PCM_FORMAT_S8,fA.at(0));

    QVector<int> fB = device.formatFromDescriptionTest(formatB);
    ASSERT_EQ(1,fB.size());
    EXPECT_EQ(SND_PCM_FORMAT_S8,fB.at(0));
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForSigned16BitLittleEndian)
{
    FormatDescription format(FormatDescription::e_DataSignedInteger,16,2,44100,true);

    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    QVector<int> f = device.formatFromDescriptionTest(format);
    ASSERT_EQ(1,f.size());
    EXPECT_EQ(SND_PCM_FORMAT_S16_LE,f.at(0));
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForSigned16BitBigEndian)
{
    FormatDescription format(FormatDescription::e_DataSignedInteger,16,2,44100,false);

    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    QVector<int> f = device.formatFromDescriptionTest(format);
    ASSERT_EQ(1,f.size());
    EXPECT_EQ(SND_PCM_FORMAT_S16_BE,f.at(0));
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForSigned18BitLittleEndian)
{
    FormatDescription format(FormatDescription::e_DataSignedInteger,18,2,44100,true);

    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    QVector<int> f = device.formatFromDescriptionTest(format);
    ASSERT_EQ(1,f.size());
    EXPECT_EQ(SND_PCM_FORMAT_S18_3LE,f.at(0));
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForSigned18BitBigEndian)
{
    FormatDescription format(FormatDescription::e_DataSignedInteger,18,2,44100,false);

    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    QVector<int> f = device.formatFromDescriptionTest(format);
    ASSERT_EQ(1,f.size());
    EXPECT_EQ(SND_PCM_FORMAT_S18_3BE,f.at(0));
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForSigned20BitLittleEndian)
{
    FormatDescription format(FormatDescription::e_DataSignedInteger,20,2,44100,true);

    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    QVector<int> f = device.formatFromDescriptionTest(format);
    ASSERT_EQ(1,f.size());
    EXPECT_EQ(SND_PCM_FORMAT_S20_3LE,f.at(0));
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForSigned20BitBigEndian)
{
    FormatDescription format(FormatDescription::e_DataSignedInteger,20,2,44100,false);

    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    QVector<int> f = device.formatFromDescriptionTest(format);
    ASSERT_EQ(1,f.size());
    EXPECT_EQ(SND_PCM_FORMAT_S20_3BE,f.at(0));
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForSigned24BitLittleEndian)
{
    FormatDescription format(FormatDescription::e_DataSignedInteger,24,2,44100,true);

    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    QVector<int> f = device.formatFromDescriptionTest(format);
    ASSERT_EQ(2,f.size());
    EXPECT_EQ(SND_PCM_FORMAT_S24_3LE,f.at(0));
    EXPECT_EQ(SND_PCM_FORMAT_S24_LE,f.at(1));
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForSigned24BitBigEndian)
{
    FormatDescription format(FormatDescription::e_DataSignedInteger,24,2,44100,false);

    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    QVector<int> f = device.formatFromDescriptionTest(format);
    ASSERT_EQ(2,f.size());
    EXPECT_EQ(SND_PCM_FORMAT_S24_3BE,f.at(0));
    EXPECT_EQ(SND_PCM_FORMAT_S24_BE,f.at(1));
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForSigned32BitLittleEndian)
{
    FormatDescription format(FormatDescription::e_DataSignedInteger,32,2,44100,true);

    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    QVector<int> f = device.formatFromDescriptionTest(format);
    ASSERT_EQ(1,f.size());
    EXPECT_EQ(SND_PCM_FORMAT_S32_LE,f.at(0));
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForSigned32BitBigEndian)
{
    FormatDescription format(FormatDescription::e_DataSignedInteger,32,2,44100,false);

    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    QVector<int> f = device.formatFromDescriptionTest(format);
    ASSERT_EQ(1,f.size());
    EXPECT_EQ(SND_PCM_FORMAT_S32_BE,f.at(0));
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForSingleFloatLittleEndian)
{
    FormatDescription format;
    format.setTypeOfData(FormatDescription::e_DataFloatSingle);
    format.setEndian(true);

    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    QVector<int> f = device.formatFromDescriptionTest(format);
    ASSERT_EQ(1,f.size());
    EXPECT_EQ(SND_PCM_FORMAT_FLOAT_LE,f.at(0));
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForSingleFloatBigEndian)
{
    FormatDescription format;
    format.setTypeOfData(FormatDescription::e_DataFloatSingle);
    format.setEndian(false);

    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    QVector<int> f = device.formatFromDescriptionTest(format);
    ASSERT_EQ(1,f.size());
    EXPECT_EQ(SND_PCM_FORMAT_FLOAT_BE,f.at(0));
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForDoubleFloatLittleEndian)
{
    FormatDescription format;
    format.setTypeOfData(FormatDescription::e_DataFloatDouble);
    format.setEndian(true);

    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    QVector<int> f = device.formatFromDescriptionTest(format);
    ASSERT_EQ(1,f.size());
    EXPECT_EQ(SND_PCM_FORMAT_FLOAT64_LE,f.at(0));
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForDoubleFloatBigEndian)
{
    FormatDescription format;
    format.setTypeOfData(FormatDescription::e_DataFloatDouble);
    format.setEndian(false);

    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    QVector<int> f = device.formatFromDescriptionTest(format);
    ASSERT_EQ(1,f.size());
    EXPECT_EQ(SND_PCM_FORMAT_FLOAT64_BE,f.at(0));
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,formatFromDescriptionForInvalid)
{
    AOQueryALSADeviceALSAFormatFromDescriptionTest device;

    FormatDescription formatA(FormatDescription::e_DataUnsignedInteger,16,2,44100,true);
    QVector<int> fA = device.formatFromDescriptionTest(formatA);
    ASSERT_EQ(0,fA.size());

    FormatDescription formatB(FormatDescription::e_DataSignedInteger,14,2,44100);
    QVector<int> fB = device.formatFromDescriptionTest(formatB);
    ASSERT_EQ(0,fB.size());
}

//-------------------------------------------------------------------------------------------

class AOQueryALSADeviceALSAHasFormatTest : public AOQueryALSA::DeviceALSA
{
    public:
        MOCK_CONST_METHOD3(printErrorOS,void(const tchar *strE,const tchar *strR,int err));
        bool testHasFormat(snd_pcm_t *handle,const FormatDescription& desc);
};

//-------------------------------------------------------------------------------------------

bool AOQueryALSADeviceALSAHasFormatTest::testHasFormat(snd_pcm_t *handle,const FormatDescription& desc)
{
    return hasFormat(handle,desc);
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,hasFormatGivenErrorInParameterAllocation)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)(6);

    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_pcm_hw_params_malloc(A<snd_pcm_hw_params_t **>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(params),Return(-1)));

    FormatDescription format(FormatDescription::e_DataSignedInteger,16,4,96000);

    AOQueryALSADeviceALSAHasFormatTest device;
    EXPECT_CALL(device,printErrorOS(StrEq("hasFormat"),StrEq("Failed to allocate ALSA hardware parameters"),Eq(-1))).Times(1);

    EXPECT_FALSE(device.testHasFormat(handle,format));

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,hasFormatGivenNothingAllocatedInParameterAllocation)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)(0);

    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_pcm_hw_params_malloc(A<snd_pcm_hw_params_t **>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(params),Return(0)));

    FormatDescription format(FormatDescription::e_DataSignedInteger,16,4,96000);

    AOQueryALSADeviceALSAHasFormatTest device;
    EXPECT_CALL(device,printErrorOS(StrEq("hasFormat"),StrEq("Failed to allocate ALSA hardware parameters"),Eq(0))).Times(1);

    EXPECT_FALSE(device.testHasFormat(handle,format));

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,hasFormatGivenFailureToGetConfigurationForPCM)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)(6);

    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_pcm_hw_params_malloc(A<snd_pcm_hw_params_t **>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(params),Return(0)));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_any(Eq(handle),Eq(params))).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_free(Eq(params))).Times(1);

    FormatDescription format(FormatDescription::e_DataSignedInteger,16,4,96000);

    AOQueryALSADeviceALSAHasFormatTest device;
    EXPECT_CALL(device,printErrorOS(StrEq("hasFormat"),StrEq("Error getting ALSA hardware parameters for PCM"),Eq(-1))).Times(1);

    EXPECT_FALSE(device.testHasFormat(handle,format));

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,hasFormatGivenRatesRestrictedToHardwareOnly)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)(6);

    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_pcm_hw_params_malloc(A<snd_pcm_hw_params_t **>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(params),Return(0)));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_any(Eq(handle),Eq(params))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_rate_resample(Eq(handle),Eq(params),Eq(0))).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_free(Eq(params))).Times(1);

    FormatDescription format(FormatDescription::e_DataSignedInteger,16,4,96000);

    AOQueryALSADeviceALSAHasFormatTest device;
    EXPECT_CALL(device,printErrorOS(StrEq("hasFormat"),StrEq("Failed to turn off ALSA software resampler"),Eq(-1))).Times(1);

    EXPECT_FALSE(device.testHasFormat(handle,format));

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,hasFormatGivenErrorRestrictingToPCMReadWriteAccess)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)(6);

    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_pcm_hw_params_malloc(A<snd_pcm_hw_params_t **>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(params),Return(0)));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_any(Eq(handle),Eq(params))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_rate_resample(Eq(handle),Eq(params),Eq(0))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_access(Eq(handle),Eq(params),Eq(SND_PCM_ACCESS_RW_INTERLEAVED))).Times(1).WillOnce(Return(-2));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_free(Eq(params))).Times(1);

    FormatDescription format(FormatDescription::e_DataSignedInteger,16,4,96000);

    AOQueryALSADeviceALSAHasFormatTest device;
    EXPECT_CALL(device,printErrorOS(StrEq("hasFormat"),StrEq("Error restricting ALSA hardware parameters to PCM read/write access"),Eq(-2))).Times(1);

    EXPECT_FALSE(device.testHasFormat(handle,format));

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,hasFormatGiven4ChannelsConfigurationNotSupported)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)(6);

    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_pcm_hw_params_malloc(A<snd_pcm_hw_params_t **>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(params),Return(0)));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_any(Eq(handle),Eq(params))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_rate_resample(Eq(handle),Eq(params),Eq(0))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_access(Eq(handle),Eq(params),Eq(SND_PCM_ACCESS_RW_INTERLEAVED))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_channels(Eq(handle),Eq(params),Eq(4))).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_free(Eq(params))).Times(1);

    FormatDescription format(FormatDescription::e_DataSignedInteger,16,4,96000);

    AOQueryALSADeviceALSAHasFormatTest device;
    EXPECT_FALSE(device.testHasFormat(handle,format));

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,hasFormatGivenByteFormatIsNotSupported)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)(6);

    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_pcm_hw_params_malloc(A<snd_pcm_hw_params_t **>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(params),Return(0)));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_any(Eq(handle),Eq(params))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_rate_resample(Eq(handle),Eq(params),Eq(0))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_access(Eq(handle),Eq(params),Eq(SND_PCM_ACCESS_RW_INTERLEAVED))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_channels(Eq(handle),Eq(params),Eq(4))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_format(Eq(handle),Eq(params),Eq(SND_PCM_FORMAT_S16_LE))).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_free(Eq(params))).Times(1);

    FormatDescription format(FormatDescription::e_DataSignedInteger,16,4,96000);

    AOQueryALSADeviceALSAHasFormatTest device;
    EXPECT_FALSE(device.testHasFormat(handle,format));

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,hasFormatGivenSampleRateCannotBeSet)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)(6);

    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_pcm_hw_params_malloc(A<snd_pcm_hw_params_t **>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(params),Return(0)));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_any(Eq(handle),Eq(params))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_rate_resample(Eq(handle),Eq(params),Eq(0))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_access(Eq(handle),Eq(params),Eq(SND_PCM_ACCESS_RW_INTERLEAVED))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_channels(Eq(handle),Eq(params),Eq(4))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_format(Eq(handle),Eq(params),Eq((snd_pcm_format_t)SND_PCM_FORMAT_S16_LE))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_rate(Eq(handle),Eq(params),Eq(96000),Eq(0))).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_free(Eq(params))).Times(1);

    FormatDescription format(FormatDescription::e_DataSignedInteger,16,4,96000);

    AOQueryALSADeviceALSAHasFormatTest device;
    EXPECT_FALSE(device.testHasFormat(handle,format));

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,hasFormatGivenDescriptionConfigurationCannotBeSet)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)(6);

    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_pcm_hw_params_malloc(A<snd_pcm_hw_params_t **>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(params),Return(0)));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_any(Eq(handle),Eq(params))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_rate_resample(Eq(handle),Eq(params),Eq(0))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_access(Eq(handle),Eq(params),Eq(SND_PCM_ACCESS_RW_INTERLEAVED))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_channels(Eq(handle),Eq(params),Eq(4))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_format(Eq(handle),Eq(params),Eq(SND_PCM_FORMAT_S16_LE))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_rate(Eq(handle),Eq(params),Eq(96000),Eq(0))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params(Eq(handle),Eq(params))).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_free(Eq(params))).Times(1);

    FormatDescription format(FormatDescription::e_DataSignedInteger,16,4,96000);

    AOQueryALSADeviceALSAHasFormatTest device;
    EXPECT_FALSE(device.testHasFormat(handle,format));

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,hasFormatGivenFormatIsSupported)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)(6);

    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_pcm_hw_params_malloc(A<snd_pcm_hw_params_t **>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(params),Return(0)));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_any(Eq(handle),Eq(params))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_rate_resample(Eq(handle),Eq(params),Eq(0))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_access(Eq(handle),Eq(params),Eq(SND_PCM_ACCESS_RW_INTERLEAVED))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_channels(Eq(handle),Eq(params),Eq(4))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_format(Eq(handle),Eq(params),Eq(SND_PCM_FORMAT_S16_LE))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_rate(Eq(handle),Eq(params),Eq(96000),Eq(0))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params(Eq(handle),Eq(params))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_free(Eq(params))).Times(1);

    FormatDescription format(FormatDescription::e_DataSignedInteger,16,4,96000);

    AOQueryALSADeviceALSAHasFormatTest device;
    EXPECT_TRUE(device.testHasFormat(handle,format));

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,hasFormatGivenMultipleByteFormatAndSecondIsSupported)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    snd_pcm_hw_params_t *params = (snd_pcm_hw_params_t *)(6);

    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_pcm_hw_params_malloc(A<snd_pcm_hw_params_t **>())).Times(2).WillRepeatedly(DoAll(SetArgPointee<0>(params),Return(0)));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_any(Eq(handle),Eq(params))).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_rate_resample(Eq(handle),Eq(params),Eq(0))).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_access(Eq(handle),Eq(params),Eq(SND_PCM_ACCESS_RW_INTERLEAVED))).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_channels(Eq(handle),Eq(params),Eq(4))).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_format(Eq(handle),Eq(params),Eq(SND_PCM_FORMAT_S24_3LE))).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_format(Eq(handle),Eq(params),Eq(SND_PCM_FORMAT_S24_LE))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_set_rate(Eq(handle),Eq(params),Eq(96000),Eq(0))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params(Eq(handle),Eq(params))).Times(1).WillOnce(Return(0));
    EXPECT_CALL(apiMock,snd_pcm_hw_params_free(Eq(params))).Times(2);

    FormatDescription format(FormatDescription::e_DataSignedInteger,24,4,96000);

    AOQueryALSADeviceALSAHasFormatTest device;
    EXPECT_TRUE(device.testHasFormat(handle,format));

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

class AOQueryALSADeviceALSADescriptionFromFormat : public AOQueryALSA::DeviceALSA
{
    public:
        bool testDescriptionFromFormat(int alsaFormat,int noChannels,int frequency,FormatDescription& desc);
};

//-------------------------------------------------------------------------------------------

bool AOQueryALSADeviceALSADescriptionFromFormat::testDescriptionFromFormat(int alsaFormat,int noChannels,int frequency,FormatDescription& desc)
{
    return descriptionFromFormat(alsaFormat,noChannels,frequency,desc);
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatSigned8Bit)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_TRUE(device.testDescriptionFromFormat(SND_PCM_FORMAT_S8,2,44100,desc));
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,desc.typeOfData());
    EXPECT_EQ(8,desc.bits());
    EXPECT_EQ(2,desc.channels());
    EXPECT_EQ(44100,desc.frequency());
    EXPECT_TRUE(desc.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatSigned16BitLittleEndian)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_TRUE(device.testDescriptionFromFormat(SND_PCM_FORMAT_S16_LE,2,44100,desc));
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,desc.typeOfData());
    EXPECT_EQ(16,desc.bits());
    EXPECT_EQ(2,desc.channels());
    EXPECT_EQ(44100,desc.frequency());
    EXPECT_TRUE(desc.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatSigned16BitBigEndian)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_TRUE(device.testDescriptionFromFormat(SND_PCM_FORMAT_S16_BE,4,192000,desc));
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,desc.typeOfData());
    EXPECT_EQ(16,desc.bits());
    EXPECT_EQ(4,desc.channels());
    EXPECT_EQ(192000,desc.frequency());
    EXPECT_FALSE(desc.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatSigned24Bit4BytesLittleEndian)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_TRUE(device.testDescriptionFromFormat(SND_PCM_FORMAT_S24_LE,8,192000,desc));
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,desc.typeOfData());
    EXPECT_EQ(24,desc.bits());
    EXPECT_EQ(8,desc.channels());
    EXPECT_EQ(192000,desc.frequency());
    EXPECT_TRUE(desc.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatSigned24Bit4BytesBigEndian)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_TRUE(device.testDescriptionFromFormat(SND_PCM_FORMAT_S24_BE,2,44100,desc));
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,desc.typeOfData());
    EXPECT_EQ(24,desc.bits());
    EXPECT_EQ(2,desc.channels());
    EXPECT_EQ(44100,desc.frequency());
    EXPECT_FALSE(desc.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatFloat32LittleEndian)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_TRUE(device.testDescriptionFromFormat(SND_PCM_FORMAT_FLOAT_LE,2,44100,desc));
    EXPECT_EQ(FormatDescription::e_DataFloatSingle,desc.typeOfData());
    EXPECT_EQ(32,desc.bits());
    EXPECT_EQ(2,desc.channels());
    EXPECT_EQ(44100,desc.frequency());
    EXPECT_TRUE(desc.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatFloat32BigEndian)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_TRUE(device.testDescriptionFromFormat(SND_PCM_FORMAT_FLOAT_BE,2,44100,desc));
    EXPECT_EQ(FormatDescription::e_DataFloatSingle,desc.typeOfData());
    EXPECT_EQ(32,desc.bits());
    EXPECT_EQ(2,desc.channels());
    EXPECT_EQ(44100,desc.frequency());
    EXPECT_FALSE(desc.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatFloat64LittleEndian)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_TRUE(device.testDescriptionFromFormat(SND_PCM_FORMAT_FLOAT64_LE,2,44100,desc));
    EXPECT_EQ(FormatDescription::e_DataFloatDouble,desc.typeOfData());
    EXPECT_EQ(64,desc.bits());
    EXPECT_EQ(2,desc.channels());
    EXPECT_EQ(44100,desc.frequency());
    EXPECT_TRUE(desc.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatFloat64BigEndian)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_TRUE(device.testDescriptionFromFormat(SND_PCM_FORMAT_FLOAT64_BE,2,44100,desc));
    EXPECT_EQ(FormatDescription::e_DataFloatDouble,desc.typeOfData());
    EXPECT_EQ(64,desc.bits());
    EXPECT_EQ(2,desc.channels());
    EXPECT_EQ(44100,desc.frequency());
    EXPECT_FALSE(desc.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatSigned24Bit3BytesLittleEndian)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_TRUE(device.testDescriptionFromFormat(SND_PCM_FORMAT_S24_3LE,2,44100,desc));
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,desc.typeOfData());
    EXPECT_EQ(24,desc.bits());
    EXPECT_EQ(2,desc.channels());
    EXPECT_EQ(44100,desc.frequency());
    EXPECT_TRUE(desc.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatSigned24Bit3BytesBigEndian)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_TRUE(device.testDescriptionFromFormat(SND_PCM_FORMAT_S24_3BE,2,44100,desc));
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,desc.typeOfData());
    EXPECT_EQ(24,desc.bits());
    EXPECT_EQ(2,desc.channels());
    EXPECT_EQ(44100,desc.frequency());
    EXPECT_FALSE(desc.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatSigned20BitLittleEndian)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_TRUE(device.testDescriptionFromFormat(SND_PCM_FORMAT_S20_3LE,2,44100,desc));
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,desc.typeOfData());
    EXPECT_EQ(20,desc.bits());
    EXPECT_EQ(2,desc.channels());
    EXPECT_EQ(44100,desc.frequency());
    EXPECT_TRUE(desc.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatSigned20BitBigEndian)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_TRUE(device.testDescriptionFromFormat(SND_PCM_FORMAT_S20_3BE,2,44100,desc));
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,desc.typeOfData());
    EXPECT_EQ(20,desc.bits());
    EXPECT_EQ(2,desc.channels());
    EXPECT_EQ(44100,desc.frequency());
    EXPECT_FALSE(desc.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatSigned18BitLittleEndian)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_TRUE(device.testDescriptionFromFormat(SND_PCM_FORMAT_S18_3LE,2,44100,desc));
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,desc.typeOfData());
    EXPECT_EQ(18,desc.bits());
    EXPECT_EQ(2,desc.channels());
    EXPECT_EQ(44100,desc.frequency());
    EXPECT_TRUE(desc.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatSigned18BitBigEndian)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_TRUE(device.testDescriptionFromFormat(SND_PCM_FORMAT_S18_3BE,2,44100,desc));
    EXPECT_EQ(FormatDescription::e_DataSignedInteger,desc.typeOfData());
    EXPECT_EQ(18,desc.bits());
    EXPECT_EQ(2,desc.channels());
    EXPECT_EQ(44100,desc.frequency());
    EXPECT_FALSE(desc.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,descriptionFromFormatUnsupported)
{
    FormatDescription desc;
    AOQueryALSADeviceALSADescriptionFromFormat device;
    ASSERT_FALSE(device.testDescriptionFromFormat(SND_PCM_FORMAT_U24_3BE,2,44100,desc));
    ASSERT_FALSE(device.testDescriptionFromFormat(SND_PCM_FORMAT_MU_LAW,2,44100,desc));
    ASSERT_FALSE(device.testDescriptionFromFormat(SND_PCM_FORMAT_UNKNOWN,2,44100,desc));
}

//-------------------------------------------------------------------------------------------

class AOQueryALSADeviceALSAQuerySupportedFormats : public AOQueryALSA::DeviceALSA
{
    public:
        virtual void testQuerySupportedFormats(snd_pcm_t *handle);
        virtual bool hasFormat(snd_pcm_t *handle,const FormatDescription& desc) const;
};

//-------------------------------------------------------------------------------------------

void AOQueryALSADeviceALSAQuerySupportedFormats::testQuerySupportedFormats(snd_pcm_t *handle)
{
    return querySupportedFormats(handle);
}

//-------------------------------------------------------------------------------------------

bool AOQueryALSADeviceALSAQuerySupportedFormats::hasFormat(snd_pcm_t *handle,const FormatDescription& desc) const
{
    bool res = false;

    if(desc.isLittleEndian())
    {
        if((desc.bits()==24 || desc.bits()==16) && desc.channels()==2 && (desc.frequency()==44100 || desc.frequency()==48000))
        {
            res = true;
        }
        else if(desc.bits()==24 && desc.channels()==8 && desc.frequency()==192000)
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,querySupportedFormats)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);

    AOQueryALSADeviceALSAQuerySupportedFormats device;

    device.testQuerySupportedFormats(handle);

    QSet<tint> frequencies = FormatDescription::setOfFrequencies();
    for(QSet<tint>::iterator ppI=frequencies.begin();ppI!=frequencies.end();ppI++)
    {
        tint freq = *ppI;
        for(tint noChannels=1;noChannels<=8;noChannels++)
        {
            for(tint noBits=8;noBits<=32;noBits+=8)
            {
                for(tint endian=0;endian<2;endian++)
                {
                    bool expect = false;

                    if(!endian)
                    {
                        if((noBits==24 || noBits==16) && noChannels==2 && (freq==44100 || freq==48000))
                        {
                            expect = true;
                        }
                        else if(noBits==24 && noChannels==8 && freq==192000)
                        {
                            expect = true;
                        }
                    }

                    FormatDescription cDesc(FormatDescription::e_DataSignedInteger,noBits,noChannels,freq,(!endian) ? true : false);
                    if(expect)
                    {
                        EXPECT_TRUE(device.isSupported(cDesc));
                        EXPECT_TRUE(device.supported().isSupported(cDesc));
                    }
                    else
                    {
                        EXPECT_FALSE(device.isSupported(cDesc));
                        EXPECT_FALSE(device.supported().isSupported(cDesc));
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

class AOQueryALSAQueryNamesTest : public AOQueryALSA
{
    public:
        MOCK_CONST_METHOD0(listOfCards,QVector<tint>());
};

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSA,queryNames)
{
    QVector<tint> cards;
    cards.append(1);
    cards.append(2);

    QString cardNameA = "Soundcard 1";
    QByteArray cardAArray = cardNameA.toLatin1();
    char *nA = cardAArray.data();
    QString cardNameB = "Soundcard 2";
    QByteArray cardBArray = cardNameB.toLatin1();
    char *nB = cardBArray.data();

    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));
    EXPECT_CALL(apiMock,snd_card_get_longname(Eq(1),A<char **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(nA),Return(0)));
    EXPECT_CALL(apiMock,snd_card_get_longname(Eq(2),A<char **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(nB),Return(0)));

    AOQueryALSAQueryNamesTest devices;
    EXPECT_CALL(devices,listOfCards()).Times(1).WillOnce(Return(cards));

    ASSERT_TRUE(devices.queryNames());

    ASSERT_EQ(2,devices.noDevices());
    EXPECT_TRUE(devices.device(0).id()=="1");
    EXPECT_TRUE(devices.device(0).name()==cardNameA);
    EXPECT_TRUE(devices.device(1).id()=="2");
    EXPECT_TRUE(devices.device(1).name()==cardNameB);

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

class AOQueryALSADeviceALSAQueryDeviceTest : public AOQueryALSA::DeviceALSA
{
    public:
        virtual bool hasFormat(snd_pcm_t *handle,const FormatDescription& desc) const;
};

//-------------------------------------------------------------------------------------------

bool AOQueryALSADeviceALSAQueryDeviceTest::hasFormat(snd_pcm_t *handle,const FormatDescription& desc) const
{
    bool res = false;

    if(desc.isLittleEndian())
    {
        if((desc.bits()==24 || desc.bits()==16) && desc.channels()==2 && (desc.frequency()==44100 || desc.frequency()==48000))
        {
            res = true;
        }
        else if(desc.bits()==24 && desc.channels()==4 && desc.frequency()==192000)
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryALSADeviceALSA,queryDevice)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);

    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_pcm_open(A<snd_pcm_t **>(),StrEq("hw:3"),Eq(SND_PCM_STREAM_PLAYBACK),Eq(0))).Times(1).WillOnce(DoAll(SetArgPointee<0>(handle),Return(0)));
    EXPECT_CALL(apiMock,snd_pcm_close(Eq(handle))).Times(1).WillOnce(Return(0));

    AOQueryALSADeviceALSAQueryDeviceTest device;

    ASSERT_TRUE(device.queryDevice(3));

    EXPECT_TRUE(device.pcmDeviceName()=="hw:3");

    EXPECT_TRUE(device.isInitialized());
    EXPECT_EQ(AOQueryDevice::Device::e_deviceALSA,device.type());

    EXPECT_FALSE(device.isFrequencySupported(8000));
    EXPECT_FALSE(device.isFrequencySupported(11025));
    EXPECT_FALSE(device.isFrequencySupported(12000));
    EXPECT_FALSE(device.isFrequencySupported(16000));
    EXPECT_FALSE(device.isFrequencySupported(22050));
    EXPECT_FALSE(device.isFrequencySupported(24000));
    EXPECT_FALSE(device.isFrequencySupported(32000));
    EXPECT_TRUE(device.isFrequencySupported(44100));
    EXPECT_TRUE(device.isFrequencySupported(48000));
    EXPECT_FALSE(device.isFrequencySupported(64000));
    EXPECT_FALSE(device.isFrequencySupported(88200));
    EXPECT_FALSE(device.isFrequencySupported(96000));
    EXPECT_FALSE(device.isFrequencySupported(176400));
    EXPECT_TRUE(device.isFrequencySupported(192000));
    EXPECT_FALSE(device.isFrequencySupported(352800));
    EXPECT_FALSE(device.isFrequencySupported(384000));
    EXPECT_FALSE(device.isFrequencySupported(705600));
    EXPECT_FALSE(device.isFrequencySupported(768000));

    EXPECT_EQ(3,device.frequencies().size());
    EXPECT_TRUE(device.frequencies().contains(44100));
    EXPECT_TRUE(device.frequencies().contains(48000));
    EXPECT_TRUE(device.frequencies().contains(192000));

    EXPECT_EQ(4,device.noChannels());

    QSet<tint> frequencies = FormatDescription::setOfFrequencies();
    for(QSet<tint>::iterator ppI=frequencies.begin();ppI!=frequencies.end();ppI++)
    {
        tint freq = *ppI;
        for(tint noChannels=1;noChannels<=8;noChannels++)
        {
            for(tint noBits=8;noBits<=32;noBits+=8)
            {
                for(tint endian=0;endian<2;endian++)
                {
                    bool expect = false;

                    if(!endian)
                    {
                        if((noBits==24 || noBits==16) && noChannels==2 && (freq==44100 || freq==48000))
                        {
                            expect = true;
                        }
                        else if(noBits==24 && noChannels==4 && freq==192000)
                        {
                            expect = true;
                        }
                    }

                    FormatDescription cDesc(FormatDescription::e_DataSignedInteger,noBits,noChannels,freq,(!endian) ? true : false);
                    if(expect)
                    {
                        EXPECT_TRUE(device.isSupported(cDesc));
                        EXPECT_TRUE(device.supported().isSupported(cDesc));
                    }
                    else
                    {
                        EXPECT_FALSE(device.isSupported(cDesc));
                        EXPECT_FALSE(device.supported().isSupported(cDesc));
                    }
                }
            }
        }
    }

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------
