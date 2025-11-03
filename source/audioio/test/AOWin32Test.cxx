#include "audioio/inc/AOWin32.h"
#include "audioio/test/ASIODriverMock.h"
#include "audioio/test/ASIODataMock.h"
#include "audioio/test/AudioHardwareBufferTester.h"
#include "audioio/inc/AOQueryWasAPI.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

class AOWin32Test : public AOWin32
{
    public:
        AOWin32Test(ASIODriver *mockDriver);
        
        bool testCreateAudioBuffers();
    protected:
    
        ASIODriver *m_mockDriver;
        virtual void printError(const tchar *strE,const tchar *strR) const;
        virtual ASIODriver *getASIODriver();
};

//-------------------------------------------------------------------------------------------

AOWin32Test::AOWin32Test(ASIODriver *mockDriver) : AOWin32(),
    m_mockDriver(mockDriver)
{}

//-------------------------------------------------------------------------------------------

bool AOWin32Test::testCreateAudioBuffers()
{
    return createAudioBuffers();
}

//-------------------------------------------------------------------------------------------

ASIODriver *AOWin32Test::getASIODriver()
{
    return m_mockDriver;
}

//-------------------------------------------------------------------------------------------

void AOWin32Test::printError(const tchar *,const tchar *) const
{}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,createAudioBuffersNoDevice)
{
    AOWin32Test audio(0);
    EXPECT_FALSE(audio.testCreateAudioBuffers());
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,createAudioBuffersFailToGetBufferSize)
{
    ASIODriverMock driver;
    EXPECT_CALL(driver,ASIOGetBufferSize(A<long *>(),A<long *>(),A<long *>(),A<long *>()))
        .Times(1).WillOnce(Return(ASE_InvalidParameter));
    
    AOWin32Test audio(&driver);
    EXPECT_FALSE(audio.testCreateAudioBuffers());
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,createAudioBuffersSuccessOnFirstCreate)
{
    ASIODriverMock driver;
    EXPECT_CALL(driver,ASIOGetBufferSize(A<long *>(),A<long *>(),A<long *>(),A<long *>()))
        .Times(1).WillOnce(DoAll(SetArgPointee<0>(1000),SetArgPointee<1>(2000),SetArgPointee<2>(1500),Return(ASE_OK)));
    EXPECT_CALL(driver,ASIOCreateBuffers(A<ASIOBufferInfo *>(),2,1500,A<ASIOCallbacks *>()))
        .Times(1).WillOnce(Return(ASE_OK));
    
    AOWin32Test audio(&driver);
    EXPECT_TRUE(audio.testCreateAudioBuffers());
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,createAudioBuffersFailWithValidPreferenceSize)
{
    ASIODriverMock driver;
    EXPECT_CALL(driver,ASIOGetBufferSize(A<long *>(),A<long *>(),A<long *>(),A<long *>()))
        .Times(1).WillOnce(DoAll(SetArgPointee<0>(1000),SetArgPointee<1>(2000),SetArgPointee<2>(1500),Return(ASE_OK)));
    EXPECT_CALL(driver,ASIOCreateBuffers(A<ASIOBufferInfo *>(),2,1500,A<ASIOCallbacks *>()))
        .Times(1).WillOnce(Return(ASE_InvalidParameter));
    
    AOWin32Test audio(&driver);
    EXPECT_FALSE(audio.testCreateAudioBuffers());    
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,createAudioBuffersFailThenSucceedAfterPrefSetToMin)
{
    ASIODriverMock driver;
    EXPECT_CALL(driver,ASIOGetBufferSize(A<long *>(),A<long *>(),A<long *>(),A<long *>()))
        .Times(1).WillOnce(DoAll(SetArgPointee<0>(1000),SetArgPointee<1>(2000),SetArgPointee<2>(500),Return(ASE_OK)));
    EXPECT_CALL(driver,ASIOCreateBuffers(A<ASIOBufferInfo *>(),2,500,A<ASIOCallbacks *>()))
        .Times(1).WillOnce(Return(ASE_InvalidParameter));
    EXPECT_CALL(driver,ASIOCreateBuffers(A<ASIOBufferInfo *>(),2,1000,A<ASIOCallbacks *>()))
        .Times(1).WillOnce(Return(ASE_OK));    
    
    AOWin32Test audio(&driver);
    EXPECT_TRUE(audio.testCreateAudioBuffers());
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,createAudioBuffersFailThenSucceedAfterPrefSetToMax)
{
    ASIODriverMock driver;
    EXPECT_CALL(driver,ASIOGetBufferSize(A<long *>(),A<long *>(),A<long *>(),A<long *>()))
        .Times(1).WillOnce(DoAll(SetArgPointee<0>(100),SetArgPointee<1>(1000),SetArgPointee<2>(2000),Return(ASE_OK)));
    EXPECT_CALL(driver,ASIOCreateBuffers(A<ASIOBufferInfo *>(),2,2000,A<ASIOCallbacks *>()))
        .Times(1).WillOnce(Return(ASE_InvalidParameter));
    EXPECT_CALL(driver,ASIOCreateBuffers(A<ASIOBufferInfo *>(),2,1000,A<ASIOCallbacks *>()))
        .Times(1).WillOnce(Return(ASE_OK));    
    
    AOWin32Test audio(&driver);
    EXPECT_TRUE(audio.testCreateAudioBuffers());
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,createAudioBuffersFailOnBothAttempts)
{
    ASIODriverMock driver;
    EXPECT_CALL(driver,ASIOGetBufferSize(A<long *>(),A<long *>(),A<long *>(),A<long *>()))
        .Times(1).WillOnce(DoAll(SetArgPointee<0>(100),SetArgPointee<1>(1000),SetArgPointee<2>(2000),Return(ASE_OK)));
    EXPECT_CALL(driver,ASIOCreateBuffers(A<ASIOBufferInfo *>(),2,2000,A<ASIOCallbacks *>()))
        .Times(1).WillOnce(Return(ASE_InvalidParameter));
    EXPECT_CALL(driver,ASIOCreateBuffers(A<ASIOBufferInfo *>(),2,1000,A<ASIOCallbacks *>()))
        .Times(1).WillOnce(Return(ASE_InvalidParameter));    
    
    AOWin32Test audio(&driver);
    EXPECT_FALSE(audio.testCreateAudioBuffers());    
}

//-------------------------------------------------------------------------------------------

class AOWin32CreateIOTimeStamp : public AOWin32
{
    public:
        MOCK_CONST_METHOD1(getTimeFromASIO,common::TimeStamp(const ASIOTimeStamp *tS));
        IOTimeStamp testCreateIOTimeStamp(const ASIOTime *sysTime) const;
};

//-------------------------------------------------------------------------------------------

IOTimeStamp AOWin32CreateIOTimeStamp::testCreateIOTimeStamp(const ASIOTime *sysTime) const
{
    return createIOTimeStamp(sysTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,createIOTimeStampWhenTimeIsValid)
{
    common::TimeStamp tStamp(3.0);

    ASIOTime sysTime;
    memset(&sysTime,0,sizeof(ASIOTime));
    sysTime.timeInfo.flags = kSystemTimeValid;
    
    AOWin32CreateIOTimeStamp audio;
    EXPECT_CALL(audio,getTimeFromASIO(&(sysTime.timeInfo.systemTime))).Times(1).WillOnce(Return(tStamp));
    
    IOTimeStamp tS = audio.testCreateIOTimeStamp(&sysTime);
    EXPECT_TRUE(tS.isValid());
    EXPECT_EQ(tStamp,tS.time());
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,createIOTimeStampWhenTimeIsNotValid)
{
    ASIOTime sysTime;
    memset(&sysTime,0,sizeof(ASIOTime));
    
    AOWin32CreateIOTimeStamp audio;
    
    IOTimeStamp tS = audio.testCreateIOTimeStamp(&sysTime);
    EXPECT_FALSE(tS.isValid());
    EXPECT_TRUE(tS.time().isEmpty());
}

//-------------------------------------------------------------------------------------------

class AOWin32WriteToAudioOutputBufferFromPartDataTest : public AOWin32
{
    public:
        AOWin32WriteToAudioOutputBufferFromPartDataTest();
        void testWriteToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,const engine::RData *data,
            tint partNumber,tint inChannelIndex,tint bufferIndex,tint outChannelIndex,tint inputSampleIndex,tint outputSampleIndex,tint amount);
};

//-------------------------------------------------------------------------------------------

AOWin32WriteToAudioOutputBufferFromPartDataTest::AOWin32WriteToAudioOutputBufferFromPartDataTest() : AOWin32()
{
    m_deviceType = AOQueryDevice::Device::e_deviceASIO;
}

//-------------------------------------------------------------------------------------------

void AOWin32WriteToAudioOutputBufferFromPartDataTest::testWriteToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,const engine::RData *data,
        tint partNumber,tint inChannelIndex,tint bufferIndex,tint outChannelIndex,tint inputSampleIndex,tint outputSampleIndex,tint amount)
{
    writeToAudioOutputBufferFromPartData(pBuffer,data,partNumber,inChannelIndex,bufferIndex,outChannelIndex,inputSampleIndex,outputSampleIndex,amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasNoDataToOutputStartToMid)
{
    static const tint c_outputSize = 6;
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff  //  5
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0x00, 0x00, 0x00, //  0
        0x00, 0x00, 0x00, //  1
        0x00, 0x00, 0x00, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff  //  5
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(0)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,0,0,3);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasNoDataToOutputMidToEnd)
{
    static const tint c_outputSize = 6;
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff  //  5
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0x00, 0x00, 0x00, //  0
        0x00, 0x00, 0x00, //  1
        0x00, 0x00, 0x00  //  2
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(0)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,0,3,3);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasNoDataToOutputInMiddle)
{
    static const tint c_outputSize = 6;
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff  //  5
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0x00, 0x00, 0x00, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff  //  5
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(0)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,10,3,1);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasNoDataToOutputFromStartToEnd)
{
    static const tint c_outputSize = 6;
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff  //  5
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0x00, 0x00, 0x00, //  0
        0x00, 0x00, 0x00, //  1
        0x00, 0x00, 0x00, //  2
        0x00, 0x00, 0x00, //  3
        0x00, 0x00, 0x00, //  4
        0x00, 0x00, 0x00  //  5
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(0)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,10,0,6);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputStartToEndToOutputStartToEnd)
{
    static const tint c_inputSize = 10;
    static const tint c_outputSize = 10;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0xff, 0xff, 0xff, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,0,0,10);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputInMiddleToOutputStartToEnd)
{
    static const tint c_inputSize = 10;
    static const tint c_outputSize = 6;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff  //  5
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0x32, 0x33, 0x34, //  0
        0x42, 0x43, 0x44, //  1
        0x52, 0x53, 0x54, //  2
        0x62, 0x63, 0x64, //  3
        0x72, 0x73, 0x74, //  4
        0x82, 0x83, 0x84  //  5
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,2,0,6);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputMiddleToEndToOutputStartToEnd)
{
    static const tint c_inputSize = 10;
    static const tint c_outputSize = 6;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff  //  5
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0x52, 0x53, 0x54, //  0
        0x62, 0x63, 0x64, //  1
        0x72, 0x73, 0x74, //  2
        0x82, 0x83, 0x84, //  3
        0x92, 0x93, 0x94, //  4
        0xa2, 0xa3, 0xa4  //  5
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,4,0,6);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputEndToOutputStartToEnd)
{
    static const tint c_inputSize = 10;
    static const tint c_outputSize = 1;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff  //  0
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0xa2, 0xa3, 0xa4  //  0
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,9,0,1);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputStartToEndToOutputStartToMid)
{
    static const tint c_inputSize = 10;
    static const tint c_outputSize = 10;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0xff, 0xff, 0xff, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0xff, 0xff, 0xff, //  5
        0xff, 0xff, 0xff, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,0,0,5);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputInMiddleToOutputStartToMid)
{
    static const tint c_inputSize = 10;
    static const tint c_outputSize = 10;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0xff, 0xff, 0xff, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0x22, 0x23, 0x24, //  0
        0x32, 0x33, 0x34, //  1
        0x42, 0x43, 0x44, //  2
        0x52, 0x53, 0x54, //  3
        0x62, 0x63, 0x64, //  4
        0x72, 0x73, 0x74, //  5
        0x82, 0x83, 0x84, //  6
        0x92, 0x93, 0x94, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,1,0,8);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputMiddleToEndToOutputStartToMid)
{
    static const tint c_inputSize = 10;
    static const tint c_outputSize = 10;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0xff, 0xff, 0xff, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0x72, 0x73, 0x74, //  0
        0x82, 0x83, 0x84, //  1
        0x92, 0x93, 0x94, //  2
        0xa2, 0xa3, 0xa4, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0xff, 0xff, 0xff, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,6,0,4);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputEndToOutputStartToMid)
{
    static const tint c_inputSize = 10;
    static const tint c_outputSize = 10;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0xff, 0xff, 0xff, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0xa2, 0xa3, 0xa4, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0xff, 0xff, 0xff, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,9,0,1);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputStartToEndToOutputMidToEnd)
{
    static const tint c_inputSize = 6;
    static const tint c_outputSize = 10;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x52, 0x53, 0x54, //  0
        0x62, 0x63, 0x64, //  1
        0x72, 0x73, 0x74, //  2
        0x82, 0x83, 0x84, //  3
        0x92, 0x93, 0x94, //  4
        0xa2, 0xa3, 0xa4  //  5
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0xff, 0xff, 0xff, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,0,4,6);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputInMiddleToOutputMidToEnd)
{
    static const tint c_inputSize = 10;
    static const tint c_outputSize = 10;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0xff, 0xff, 0xff, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0x42, 0x43, 0x44, //  6
        0x52, 0x53, 0x54, //  7
        0x62, 0x63, 0x64, //  8
        0x72, 0x73, 0x74  //  9
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,3,6,4);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputMiddleToEndToOutputMidToEnd)
{
    static const tint c_inputSize = 10;
    static const tint c_outputSize = 9;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0xff, 0xff, 0xff, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff //  8
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0x82, 0x83, 0x84, //  6
        0x92, 0x93, 0x94, //  7
        0xa2, 0xa3, 0xa4  //  8
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,7,6,3);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputEndToOutputMidToEnd)
{
    static const tint c_inputSize = 10;
    static const tint c_outputSize = 2;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff  //  1
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xa2, 0xa3, 0xa4  //  1
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,9,1,1);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputStartToEndToOutputInMiddle)
{
    static const tint c_inputSize = 10;
    static const tint c_outputSize = 10;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0xff, 0xff, 0xff, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0x12, 0x13, 0x14, //  3
        0x22, 0x23, 0x24, //  4
        0x32, 0x33, 0x34, //  5
        0x42, 0x43, 0x44, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,0,3,4);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputInMiddleToOutputInMiddle)
{
    static const tint c_inputSize = 10;
    static const tint c_outputSize = 10;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0xff, 0xff, 0xff, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0xff, 0xff, 0xff, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,3,3,3);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputMiddleToEndToOutputInMiddle)
{
    static const tint c_inputSize = 10;
    static const tint c_outputSize = 10;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff, //  2
        0xff, 0xff, 0xff, //  3
        0xff, 0xff, 0xff, //  4
        0xff, 0xff, 0xff, //  5
        0xff, 0xff, 0xff, //  6
        0xff, 0xff, 0xff, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0x52, 0x53, 0x54, //  2
        0x62, 0x63, 0x64, //  3
        0x72, 0x73, 0x74, //  4
        0x82, 0x83, 0x84, //  5
        0x92, 0x93, 0x94, //  6
        0xa2, 0xa3, 0xa4, //  7
        0xff, 0xff, 0xff, //  8
        0xff, 0xff, 0xff  //  9
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,4,2,6);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,writeToAudioOutputBufferFromPartDataChannelHasDataFromInputEndToEndToOutputInMiddle)
{
    static const tint c_inputSize = 10;
    static const tint c_outputSize = 10;

    const tubyte c_inputData[c_inputSize * 3] = {
        0x12, 0x13, 0x14, //  0
        0x22, 0x23, 0x24, //  1
        0x32, 0x33, 0x34, //  2
        0x42, 0x43, 0x44, //  3
        0x52, 0x53, 0x54, //  4
        0x62, 0x63, 0x64, //  5
        0x72, 0x73, 0x74, //  6
        0x82, 0x83, 0x84, //  7
        0x92, 0x93, 0x94, //  8
        0xa2, 0xa3, 0xa4  //  9
    };
    
    const tubyte c_originalOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xff, 0xff, 0xff, //  1
        0xff, 0xff, 0xff  //  2
    };

    const tubyte c_expectOutputBuffer[c_outputSize * 3] = {
        0xff, 0xff, 0xff, //  0
        0xa2, 0xa3, 0xa4, //  1
        0xff, 0xff, 0xff  //  2
    };
    
    tbyte actualOutput[c_outputSize * 3];
    memcpy(actualOutput,c_originalOutputBuffer,c_outputSize * 3);
    
    ASIODataMock data;
    EXPECT_CALL(data,asioDataConst(3,2)).Times(1).WillOnce(Return(reinterpret_cast<const void *>(c_inputData)));
    EXPECT_CALL(data,getSampleSize()).Times(1).WillOnce(Return(3));
    
    AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,sampleSize(4)).Times(2).WillRepeatedly(Return(3));
    EXPECT_CALL(buffer,numberOfChannelsInBuffer(4)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(buffer,buffer(4)).Times(1).WillOnce(Return(actualOutput));
    
    AOWin32WriteToAudioOutputBufferFromPartDataTest audio;
    
    audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,3,4,-1,9,1,1);
    
    EXPECT_EQ(0,memcmp(c_expectOutputBuffer,actualOutput,c_outputSize * 3));
}

//-------------------------------------------------------------------------------------------

class AOWin32CopyDeviceInformationTest : public AOWin32
{
    public:
        virtual QSharedPointer<AOQueryDevice::Device> copyDeviceInformation(const AOQueryDevice::Device& iDevice);
};

//-------------------------------------------------------------------------------------------

QSharedPointer<AOQueryDevice::Device> AOWin32CopyDeviceInformationTest::copyDeviceInformation(const AOQueryDevice::Device& iDevice)
{
    return AOWin32::copyDeviceInformation(iDevice);
}

//-------------------------------------------------------------------------------------------

TEST(AOWin32,copyDeviceInformationASIO)
{
    AOQueryDevice::Device iDevice(AOQueryDevice::Device::e_deviceASIO);
    iDevice.setInitialized();
    iDevice.id() = "Device";
    iDevice.name() = "SoundBlaster";
    iDevice.addFrequency(44100);
    iDevice.addFrequency(48000);
    iDevice.setNoChannels(4);
    
    AOWin32CopyDeviceInformationTest audio;
    
    QSharedPointer<AOQueryDevice::Device> pDevice = audio.copyDeviceInformation(iDevice);
    ASSERT_FALSE(pDevice.isNull());
    
    EXPECT_TRUE(pDevice->isInitialized());
    EXPECT_EQ(AOQueryDevice::Device::e_deviceASIO,pDevice->type());
    EXPECT_TRUE(pDevice->id()=="Device");
    EXPECT_TRUE(pDevice->name()=="SoundBlaster");
    EXPECT_TRUE(pDevice->isFrequencySupported(44100));
    EXPECT_TRUE(pDevice->isFrequencySupported(48000));
    EXPECT_FALSE(pDevice->isFrequencySupported(192000));
    ASSERT_EQ(4,pDevice->noChannels());
}

//-------------------------------------------------------------------------------------------


TEST(AOWin32,copyDeviceInformationWasAPI)
{
    AOQueryWasAPI::DeviceWasAPI iDevice;
    iDevice.setInitialized();
    iDevice.id() = "Device";
    iDevice.name() = "SoundBlaster";
    iDevice.addFrequency(44100);
    iDevice.addFrequency(48000);
    iDevice.setNoChannels(4);

    AOWin32CopyDeviceInformationTest audio;

    QSharedPointer<AOQueryDevice::Device> pDevice = audio.copyDeviceInformation(iDevice);
    ASSERT_FALSE(pDevice.isNull());

    EXPECT_TRUE(pDevice->isInitialized());
    EXPECT_EQ(AOQueryDevice::Device::e_deviceWasAPI,pDevice->type());
    EXPECT_TRUE(pDevice->id()=="Device");
    EXPECT_TRUE(pDevice->name()=="SoundBlaster");
    EXPECT_TRUE(pDevice->isFrequencySupported(44100));
    EXPECT_TRUE(pDevice->isFrequencySupported(48000));
    EXPECT_FALSE(pDevice->isFrequencySupported(192000));
    ASSERT_EQ(4,pDevice->noChannels());
}

//-------------------------------------------------------------------------------------------
