
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "engine/inc/InterleavedCodec.h"

using namespace omega;
using namespace omega::engine;
using namespace testing;

//-------------------------------------------------------------------------------------------

class InterleavedCodecMock : public InterleavedCodec
{
    public:
        InterleavedCodecMock(const common::TimeStamp& sTime);
        virtual ~InterleavedCodecMock();
        
        MOCK_METHOD1(open,bool(const QString& name));
        MOCK_METHOD0(close,void());
        MOCK_CONST_METHOD0(isSeek,bool());
        MOCK_METHOD1(seek,bool(const common::TimeStamp& v));
        MOCK_CONST_METHOD0(isComplete,bool());
        MOCK_CONST_METHOD0(isRemote,bool());
        MOCK_METHOD1(isBuffered,bool(tfloat32& percent));
        MOCK_CONST_METHOD0(bitrate,tint());
        MOCK_CONST_METHOD0(frequency,tint());
        MOCK_CONST_METHOD0(noChannels,tint());
        MOCK_CONST_METHOD0(length,common::TimeStamp());
        
        MOCK_METHOD1(decodeNextPacket,bool(int& outLen));
        MOCK_METHOD0(getPacketBuffer,char *());
        
        virtual int bytesPerSample();
        virtual sample_t readSample(char *buffer);

};

//-------------------------------------------------------------------------------------------

InterleavedCodecMock::InterleavedCodecMock(const common::TimeStamp& sTime) : InterleavedCodec(Codec::e_codecUnknown)
{
    m_time = sTime;
}

//-------------------------------------------------------------------------------------------

InterleavedCodecMock::~InterleavedCodecMock()
{}

//-------------------------------------------------------------------------------------------

int InterleavedCodecMock::bytesPerSample()
{
    return sizeof(tint16);
}

//-------------------------------------------------------------------------------------------

sample_t InterleavedCodecMock::readSample(char *buffer)
{
    tint16 *x = reinterpret_cast<tint16 *>(buffer);
    sample_t y = static_cast<sample_t>(x[0]);
    return y;
}

//-------------------------------------------------------------------------------------------

TEST(InterleavedCodec,nextGivenCodecNotInitialized)
{
    engine::RData rData(7,2,2);
    
    common::TimeStamp startTime;
    startTime = 4.0;
        
    InterleavedCodecMock codec(startTime);
    
    ASSERT_FALSE(codec.next(rData));
}

//-------------------------------------------------------------------------------------------

TEST(InterleavedCodec,nextWithLargeDataAndSmallDecodeBuffer)
{
    const tfloat64 c_Tolerance = 0.0000001;
    const tint c_NumberOfSamples = 7;
    const tint c_SampleRate = 512;
    
    tint16 dataA[6] = { -9, -8, -7, -6, -5, -4 };
    tint16 dataB[6] = { -4, -3, -2, -1,  1,  2 };
    tint16 dataC[6] = {  3,  4,  5,  6,  7,  8 };
    
    engine::RData rData(c_NumberOfSamples,2,2);
    
    common::TimeStamp startTime,endTime;
    startTime = 4.0;
    endTime = startTime + static_cast<tfloat64>(c_NumberOfSamples) / static_cast<tfloat64>(c_SampleRate);
    
    InterleavedCodecMock codec(startTime);
    
    EXPECT_CALL(codec,decodeNextPacket(A<int&>())).Times(3).WillRepeatedly(DoAll(SetArgReferee<0>(3),Return(true)));
    EXPECT_CALL(codec,getPacketBuffer()).Times(3).WillOnce(Return((char *)dataA))
        .WillOnce(Return((char *)dataB))
        .WillOnce(Return((char *)dataC));
    EXPECT_CALL(codec,noChannels()).WillRepeatedly(Return(2));
    EXPECT_CALL(codec,frequency()).WillRepeatedly(Return(c_SampleRate));
    
    ASSERT_TRUE(codec.init());
    ASSERT_TRUE(codec.next(rData));
    
    EXPECT_TRUE(rData.noParts()==1);
    EXPECT_NEAR(static_cast<tfloat64>(rData.start()),static_cast<tfloat64>(startTime),c_Tolerance);
    EXPECT_NEAR(static_cast<tfloat64>(rData.end()),static_cast<tfloat64>(endTime),c_Tolerance);
    
    engine::RData::Part& part = rData.part(0);
    EXPECT_EQ(c_NumberOfSamples,part.length());
    EXPECT_NEAR(static_cast<tfloat64>(rData.start()),static_cast<tfloat64>(startTime),c_Tolerance);
    EXPECT_NEAR(static_cast<tfloat64>(rData.end()),static_cast<tfloat64>(endTime),c_Tolerance);
    
    const sample_t *partData = rData.partData(0);
    EXPECT_NEAR(partData[ 0],-9.0,c_Tolerance); // 0
    EXPECT_NEAR(partData[ 1],-8.0,c_Tolerance);
    EXPECT_NEAR(partData[ 2],-7.0,c_Tolerance); // 1
    EXPECT_NEAR(partData[ 3],-6.0,c_Tolerance);
    EXPECT_NEAR(partData[ 4],-5.0,c_Tolerance); // 2
    EXPECT_NEAR(partData[ 5],-4.0,c_Tolerance);
    EXPECT_NEAR(partData[ 6],-4.0,c_Tolerance); // 3
    EXPECT_NEAR(partData[ 7],-3.0,c_Tolerance);
    EXPECT_NEAR(partData[ 8],-2.0,c_Tolerance); // 4
    EXPECT_NEAR(partData[ 9],-1.0,c_Tolerance);
    EXPECT_NEAR(partData[10], 1.0,c_Tolerance); // 5
    EXPECT_NEAR(partData[11], 2.0,c_Tolerance);
    EXPECT_NEAR(partData[12], 3.0,c_Tolerance); // 6
    EXPECT_NEAR(partData[13], 4.0,c_Tolerance);
}

//-------------------------------------------------------------------------------------------

TEST(InterleavedCodec,nextWithMediumDataAndSmallDecodeBufferWithSecondNextCallOffset)
{
    const tfloat64 c_Tolerance = 0.0000001;
    const tint c_NumberOfSamples = 4;
    const tint c_SampleRate = 512;
    
    tint16 dataA[6] = { -9, -8, -7, -6, -5, -4 };
    tint16 dataB[6] = { -4, -3, -2, -1,  1,  2 };
    tint16 dataC[6] = {  3,  4,  5,  6,  7,  8 };
    
    engine::RData rDataA(c_NumberOfSamples,2,2);
    engine::RData rDataB(c_NumberOfSamples,2,2);
    
    common::TimeStamp startTime,endTimeA,endTimeB;
    startTime = 4.0;
    endTimeA = startTime + static_cast<tfloat64>(c_NumberOfSamples) / static_cast<tfloat64>(c_SampleRate);
    endTimeB = endTimeA  + static_cast<tfloat64>(c_NumberOfSamples) / static_cast<tfloat64>(c_SampleRate);
    
    InterleavedCodecMock codec(startTime);
    
    EXPECT_CALL(codec,decodeNextPacket(A<int&>())).Times(3).WillRepeatedly(DoAll(SetArgReferee<0>(3),Return(true)));
    EXPECT_CALL(codec,getPacketBuffer()).Times(3).WillOnce(Return((char *)dataA))
        .WillOnce(Return((char *)dataB))
        .WillOnce(Return((char *)dataC));
    EXPECT_CALL(codec,noChannels()).WillRepeatedly(Return(2));
    EXPECT_CALL(codec,frequency()).WillRepeatedly(Return(c_SampleRate));
    
    ASSERT_TRUE(codec.init());
    ASSERT_TRUE(codec.next(rDataA));
    ASSERT_TRUE(codec.next(rDataB));
    
    EXPECT_TRUE(rDataA.noParts()==1);
    EXPECT_NEAR(static_cast<tfloat64>(rDataA.start()),static_cast<tfloat64>(startTime),c_Tolerance);
    EXPECT_NEAR(static_cast<tfloat64>(rDataA.end()),static_cast<tfloat64>(endTimeA),c_Tolerance);
    
    engine::RData::Part& partA = rDataA.part(0);
    EXPECT_EQ(c_NumberOfSamples,partA.length());
    EXPECT_NEAR(static_cast<tfloat64>(partA.start()),static_cast<tfloat64>(startTime),c_Tolerance);
    EXPECT_NEAR(static_cast<tfloat64>(partA.end()),static_cast<tfloat64>(endTimeA),c_Tolerance);
    
    const sample_t *partDataA = rDataA.partData(0);
    EXPECT_NEAR(partDataA[ 0],-9.0,c_Tolerance); // 0
    EXPECT_NEAR(partDataA[ 1],-8.0,c_Tolerance);
    EXPECT_NEAR(partDataA[ 2],-7.0,c_Tolerance); // 1
    EXPECT_NEAR(partDataA[ 3],-6.0,c_Tolerance);
    EXPECT_NEAR(partDataA[ 4],-5.0,c_Tolerance); // 2
    EXPECT_NEAR(partDataA[ 5],-4.0,c_Tolerance);
    EXPECT_NEAR(partDataA[ 6],-4.0,c_Tolerance); // 3
    EXPECT_NEAR(partDataA[ 7],-3.0,c_Tolerance);
    
    EXPECT_TRUE(rDataB.noParts()==1);
    EXPECT_NEAR(static_cast<tfloat64>(rDataB.start()),static_cast<tfloat64>(endTimeA),c_Tolerance);
    EXPECT_NEAR(static_cast<tfloat64>(rDataB.end()),static_cast<tfloat64>(endTimeB),c_Tolerance);
    
    engine::RData::Part& partB = rDataB.part(0);
    EXPECT_EQ(c_NumberOfSamples,partB.length());
    EXPECT_NEAR(static_cast<tfloat64>(partB.start()),static_cast<tfloat64>(endTimeA),c_Tolerance);
    EXPECT_NEAR(static_cast<tfloat64>(partB.end()),static_cast<tfloat64>(endTimeB),c_Tolerance);
    
    const sample_t *partDataB = rDataB.partData(0);
    EXPECT_NEAR(partDataB[ 0],-2.0,c_Tolerance); // 0
    EXPECT_NEAR(partDataB[ 1],-1.0,c_Tolerance);
    EXPECT_NEAR(partDataB[ 2], 1.0,c_Tolerance); // 1
    EXPECT_NEAR(partDataB[ 3], 2.0,c_Tolerance);
    EXPECT_NEAR(partDataB[ 4], 3.0,c_Tolerance); // 2
    EXPECT_NEAR(partDataB[ 5], 4.0,c_Tolerance);
    EXPECT_NEAR(partDataB[ 6], 5.0,c_Tolerance); // 3
    EXPECT_NEAR(partDataB[ 7], 6.0,c_Tolerance);
}

//-------------------------------------------------------------------------------------------

TEST(InterleavedCodec,nextGivenThatDataAlreadyHasPartPreviouslyFilled)
{
    const tfloat64 c_Tolerance = 0.0000001;
    const tint c_NumberOfSamples = 9 + 4;
    const tint c_SampleRate = 512;
    
    tint16 dataA[6] = { -9, -8, -7, -6, -5, -4 };
    tint16 dataB[6] = { -4, -3, -2, -1,  1,  2 };
    tint16 dataC[6] = {  3,  4,  5,  6,  7,  8 };
    
    engine::RData rData(c_NumberOfSamples,2,2);
    
    common::TimeStamp startTimeA,endTimeA,startTimeB,endTimeB;
    startTimeA = 8.0;
    endTimeA = startTimeA + static_cast<tfloat64>(4) / static_cast<tfloat64>(c_SampleRate);
    startTimeB = 4.0;
    endTimeB = startTimeB  + static_cast<tfloat64>(9) / static_cast<tfloat64>(c_SampleRate);
    
    rData.start() = startTimeA;
    engine::RData::Part& partF = rData.nextPart();
    partF.length() = 4;
    partF.start() = startTimeA;
    partF.end() = endTimeA;
    partF.done() = true;
    rData.end() = endTimeA;
    
    InterleavedCodecMock codec(startTimeB);
    
    EXPECT_CALL(codec,decodeNextPacket(A<int&>())).Times(3).WillRepeatedly(DoAll(SetArgReferee<0>(3),Return(true)));
    EXPECT_CALL(codec,getPacketBuffer()).Times(3).WillOnce(Return((char *)dataA))
        .WillOnce(Return((char *)dataB))
        .WillOnce(Return((char *)dataC));
    EXPECT_CALL(codec,noChannels()).WillRepeatedly(Return(2));
    EXPECT_CALL(codec,frequency()).WillRepeatedly(Return(c_SampleRate));
    
    ASSERT_TRUE(codec.init());
    ASSERT_TRUE(codec.next(rData));
    
    EXPECT_TRUE(rData.noParts()==2);
    EXPECT_NEAR(static_cast<tfloat64>(rData.start()),static_cast<tfloat64>(startTimeA),c_Tolerance);
    EXPECT_NEAR(static_cast<tfloat64>(rData.end()),static_cast<tfloat64>(endTimeB),c_Tolerance);
    
    engine::RData::Part& partA = rData.part(0);
    EXPECT_EQ(4,partA.length());
    EXPECT_NEAR(static_cast<tfloat64>(partA.start()),static_cast<tfloat64>(startTimeA),c_Tolerance);
    EXPECT_NEAR(static_cast<tfloat64>(partA.end()),static_cast<tfloat64>(endTimeA),c_Tolerance);

    engine::RData::Part& partB = rData.part(1);
    EXPECT_EQ(9,partB.length());
    EXPECT_NEAR(static_cast<tfloat64>(partB.start()),static_cast<tfloat64>(startTimeB),c_Tolerance);
    EXPECT_NEAR(static_cast<tfloat64>(partB.end()),static_cast<tfloat64>(endTimeB),c_Tolerance);
    
    const sample_t *partData = rData.partData(1);
    EXPECT_NEAR(partData[ 0],-9.0,c_Tolerance); // 0
    EXPECT_NEAR(partData[ 1],-8.0,c_Tolerance);
    EXPECT_NEAR(partData[ 2],-7.0,c_Tolerance); // 1
    EXPECT_NEAR(partData[ 3],-6.0,c_Tolerance);
    EXPECT_NEAR(partData[ 4],-5.0,c_Tolerance); // 2
    EXPECT_NEAR(partData[ 5],-4.0,c_Tolerance);
    EXPECT_NEAR(partData[ 6],-4.0,c_Tolerance); // 3
    EXPECT_NEAR(partData[ 7],-3.0,c_Tolerance);
    EXPECT_NEAR(partData[ 8],-2.0,c_Tolerance); // 4
    EXPECT_NEAR(partData[ 9],-1.0,c_Tolerance);
    EXPECT_NEAR(partData[10], 1.0,c_Tolerance); // 5
    EXPECT_NEAR(partData[11], 2.0,c_Tolerance); 
    EXPECT_NEAR(partData[12], 3.0,c_Tolerance); // 6
    EXPECT_NEAR(partData[13], 4.0,c_Tolerance);
    EXPECT_NEAR(partData[14], 5.0,c_Tolerance); // 7
    EXPECT_NEAR(partData[15], 6.0,c_Tolerance);
    EXPECT_NEAR(partData[16], 7.0,c_Tolerance); // 8
    EXPECT_NEAR(partData[17], 8.0,c_Tolerance);
}

//-------------------------------------------------------------------------------------------

TEST(InterleavedCodec,nextWithMediumDataAndLargeDecodeBufferWithSecondNextCallOffset)
{
    const tfloat64 c_Tolerance = 0.0000001;
    const tint c_NumberOfSamples = 4;
    const tint c_SampleRate = 512;
    
    tint16 dataA[18] = { -9, -8, -7, -6, -5, -4, -4, -3, -2, -1,  1,  2, 3,  4,  5,  6,  7,  8 };
    
    engine::RData rDataA(c_NumberOfSamples,2,2);
    engine::RData rDataB(c_NumberOfSamples,2,2);
    
    common::TimeStamp startTime,endTimeA,endTimeB;
    startTime = 4.0;
    endTimeA = startTime + static_cast<tfloat64>(c_NumberOfSamples) / static_cast<tfloat64>(c_SampleRate);
    endTimeB = endTimeA  + static_cast<tfloat64>(c_NumberOfSamples) / static_cast<tfloat64>(c_SampleRate);
    
    InterleavedCodecMock codec(startTime);
    
    EXPECT_CALL(codec,decodeNextPacket(A<int&>())).Times(1).WillOnce(DoAll(SetArgReferee<0>(9),Return(true)));
    EXPECT_CALL(codec,getPacketBuffer()).Times(1).WillOnce(Return((char *)dataA));
    EXPECT_CALL(codec,noChannels()).WillRepeatedly(Return(2));
    EXPECT_CALL(codec,frequency()).WillRepeatedly(Return(c_SampleRate));
    
    ASSERT_TRUE(codec.init());
    ASSERT_TRUE(codec.next(rDataA));
    ASSERT_TRUE(codec.next(rDataB));
    
    EXPECT_TRUE(rDataA.noParts()==1);
    EXPECT_NEAR(static_cast<tfloat64>(rDataA.start()),static_cast<tfloat64>(startTime),c_Tolerance);
    EXPECT_NEAR(static_cast<tfloat64>(rDataA.end()),static_cast<tfloat64>(endTimeA),c_Tolerance);
    
    engine::RData::Part& partA = rDataA.part(0);
    EXPECT_EQ(c_NumberOfSamples,partA.length());
    EXPECT_NEAR(static_cast<tfloat64>(partA.start()),static_cast<tfloat64>(startTime),c_Tolerance);
    EXPECT_NEAR(static_cast<tfloat64>(partA.end()),static_cast<tfloat64>(endTimeA),c_Tolerance);
    
    const sample_t *partDataA = rDataA.partData(0);
    EXPECT_NEAR(partDataA[ 0],-9.0,c_Tolerance); // 0
    EXPECT_NEAR(partDataA[ 1],-8.0,c_Tolerance);
    EXPECT_NEAR(partDataA[ 2],-7.0,c_Tolerance); // 1
    EXPECT_NEAR(partDataA[ 3],-6.0,c_Tolerance);
    EXPECT_NEAR(partDataA[ 4],-5.0,c_Tolerance); // 2
    EXPECT_NEAR(partDataA[ 5],-4.0,c_Tolerance);
    EXPECT_NEAR(partDataA[ 6],-4.0,c_Tolerance); // 3
    EXPECT_NEAR(partDataA[ 7],-3.0,c_Tolerance);
    
    EXPECT_TRUE(rDataB.noParts()==1);
    EXPECT_NEAR(static_cast<tfloat64>(rDataB.start()),static_cast<tfloat64>(endTimeA),c_Tolerance);
    EXPECT_NEAR(static_cast<tfloat64>(rDataB.end()),static_cast<tfloat64>(endTimeB),c_Tolerance);
    
    engine::RData::Part& partB = rDataB.part(0);
    EXPECT_EQ(c_NumberOfSamples,partB.length());
    EXPECT_NEAR(static_cast<tfloat64>(partB.start()),static_cast<tfloat64>(endTimeA),c_Tolerance);
    EXPECT_NEAR(static_cast<tfloat64>(partB.end()),static_cast<tfloat64>(endTimeB),c_Tolerance);
    
    const sample_t *partDataB = rDataB.partData(0);
    EXPECT_NEAR(partDataB[ 0],-2.0,c_Tolerance); // 0
    EXPECT_NEAR(partDataB[ 1],-1.0,c_Tolerance);
    EXPECT_NEAR(partDataB[ 2], 1.0,c_Tolerance); // 1
    EXPECT_NEAR(partDataB[ 3], 2.0,c_Tolerance);
    EXPECT_NEAR(partDataB[ 4], 3.0,c_Tolerance); // 2
    EXPECT_NEAR(partDataB[ 5], 4.0,c_Tolerance);
    EXPECT_NEAR(partDataB[ 6], 5.0,c_Tolerance); // 3
    EXPECT_NEAR(partDataB[ 7], 6.0,c_Tolerance);
}

//-------------------------------------------------------------------------------------------

TEST(InterleavedCodec,nextWithLargeDataAndSmallDecodeBufferAndEndOfCodec)
{
    const tfloat64 c_Tolerance = 0.0000001;
    const tint c_NumberOfSamples = 7;
    const tint c_SampleRate = 512;
    
    tint16 dataA[6] = { -9, -8, -7, -6, -5, -4 };
    tint16 dataB[6] = { -4, -3, -2, -1,  1,  2 };
    
    engine::RData rData(c_NumberOfSamples,2,2);
    
    common::TimeStamp startTime,endTime;
    startTime = 4.0;
    endTime = startTime + static_cast<tfloat64>(6) / static_cast<tfloat64>(c_SampleRate);
    
    InterleavedCodecMock codec(startTime);
    
    EXPECT_CALL(codec,decodeNextPacket(A<int&>())).Times(3)
        .WillOnce(DoAll(SetArgReferee<0>(3),Return(true)))
        .WillOnce(DoAll(SetArgReferee<0>(3),Return(true)))
        .WillOnce(DoAll(SetArgReferee<0>(0),Return(false)));
    EXPECT_CALL(codec,getPacketBuffer()).Times(2).WillOnce(Return((char *)dataA))
        .WillOnce(Return((char *)dataB));
    EXPECT_CALL(codec,noChannels()).WillRepeatedly(Return(2));
    EXPECT_CALL(codec,frequency()).WillRepeatedly(Return(c_SampleRate));
    
    ASSERT_TRUE(codec.init());
    ASSERT_FALSE(codec.next(rData));
    
    EXPECT_TRUE(rData.noParts()==1);
    EXPECT_NEAR(static_cast<tfloat64>(rData.start()),static_cast<tfloat64>(startTime),c_Tolerance);
    EXPECT_NEAR(static_cast<tfloat64>(rData.end()),static_cast<tfloat64>(endTime),c_Tolerance);
    
    engine::RData::Part& part = rData.part(0);
    EXPECT_EQ(6,part.length());
    EXPECT_NEAR(static_cast<tfloat64>(rData.start()),static_cast<tfloat64>(startTime),c_Tolerance);
    EXPECT_NEAR(static_cast<tfloat64>(rData.end()),static_cast<tfloat64>(endTime),c_Tolerance);
    
    const sample_t *partData = rData.partData(0);
    EXPECT_NEAR(partData[ 0],-9.0,c_Tolerance); // 0
    EXPECT_NEAR(partData[ 1],-8.0,c_Tolerance);
    EXPECT_NEAR(partData[ 2],-7.0,c_Tolerance); // 1
    EXPECT_NEAR(partData[ 3],-6.0,c_Tolerance);
    EXPECT_NEAR(partData[ 4],-5.0,c_Tolerance); // 2
    EXPECT_NEAR(partData[ 5],-4.0,c_Tolerance);
    EXPECT_NEAR(partData[ 6],-4.0,c_Tolerance); // 3
    EXPECT_NEAR(partData[ 7],-3.0,c_Tolerance);
    EXPECT_NEAR(partData[ 8],-2.0,c_Tolerance); // 4
    EXPECT_NEAR(partData[ 9],-1.0,c_Tolerance);
    EXPECT_NEAR(partData[10], 1.0,c_Tolerance); // 5
    EXPECT_NEAR(partData[11], 2.0,c_Tolerance); 
}

//-------------------------------------------------------------------------------------------
