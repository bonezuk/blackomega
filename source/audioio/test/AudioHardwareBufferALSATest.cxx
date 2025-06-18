#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/AudioHardwareBufferALSA.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithSigned8BitFormat)
{
    tint len = 40 * 2 * 1;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_S8,40,2,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(1,buffer.sampleSize(0));
    EXPECT_EQ(2,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(40,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(40 * 2 * 1,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithSigned16BitLittleEndianFormat)
{
    tint len = 40 * 2 * 2;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_S16_LE,40,2,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(2,buffer.sampleSize(0));
    EXPECT_EQ(2,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(40,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(40 * 2 * 2,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithSigned16BitBigEndianFormat)
{
    tint len = 80 * 4 * 2;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_S16_BE,80,4,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(2,buffer.sampleSize(0));
    EXPECT_EQ(4,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(80,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(80 * 4 * 2,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithSigned24BitLittleEndianIn4BytesFormat)
{
    tint len = 80 * 1 * 4;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_S24_LE,80,1,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(4,buffer.sampleSize(0));
    EXPECT_EQ(1,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(80,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(80 * 1 * 4,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithSigned24BitBigEndianIn4BytesFormat)
{
    tint len = 80 * 8 * 4;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_S24_BE,80,8,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(4,buffer.sampleSize(0));
    EXPECT_EQ(8,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(80,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(80 * 8 * 4,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithSigned32BitLittleEndianFormat)
{
    tint len = 80 * 2 * 4;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_S32_LE,80,2,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(4,buffer.sampleSize(0));
    EXPECT_EQ(2,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(80,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(80 * 2 * 4,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithSigned32BitBigEndianFormat)
{
    tint len = 80 * 3 * 4;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_S32_BE,80,3,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(4,buffer.sampleSize(0));
    EXPECT_EQ(3,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(80,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(80 * 3 * 4,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithFloatLittleEndianFormat)
{
    tint len = 160 * 2 * 4;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_FLOAT_LE,160,2,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(4,buffer.sampleSize(0));
    EXPECT_EQ(2,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(160,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(160 * 2 * 4,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithFloatBigEndianFormat)
{
    tint len = 160 * 2 * 4;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_FLOAT_BE,160,2,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(4,buffer.sampleSize(0));
    EXPECT_EQ(2,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(160,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(160 * 2 * 4,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithFloat64LittleEndianFormat)
{
    tint len = 160 * 2 * 8;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_FLOAT64_LE,160,2,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(8,buffer.sampleSize(0));
    EXPECT_EQ(2,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(160,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(160 * 2 * 8,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithFloat64BigEndianFormat)
{
    tint len = 160 * 2 * 8;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_FLOAT64_BE,160,2,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(8,buffer.sampleSize(0));
    EXPECT_EQ(2,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(160,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(160 * 2 * 8,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithSigned24BitLittleEndianIn3BytesFormat)
{
    tint len = 160 * 2 * 3;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_S24_3LE,160,2,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(3,buffer.sampleSize(0));
    EXPECT_EQ(2,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(160,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(160 * 2 * 3,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithSigned24BitBigEndianIn3BytesFormat)
{
    tint len = 160 * 2 * 3;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_S24_3BE,160,2,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(3,buffer.sampleSize(0));
    EXPECT_EQ(2,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(160,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(160 * 2 * 3,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithSigned20BitLittleEndianFormat)
{
    tint len = 160 * 2 * 3;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_S20_3LE,160,2,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(3,buffer.sampleSize(0));
    EXPECT_EQ(2,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(160,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(160 * 2 * 3,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithSigned20BitBigEndianFormat)
{
    tint len = 160 * 2 * 3;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_S20_3BE,160,2,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(3,buffer.sampleSize(0));
    EXPECT_EQ(2,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(160,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(160 * 2 * 3,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithSigned18BitLittleEndianFormat)
{
    tint len = 160 * 2 * 3;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_S18_3LE,160,2,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(3,buffer.sampleSize(0));
    EXPECT_EQ(2,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(160,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(160 * 2 * 3,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferALSA,bufferWithSigned18BitBigEndianFormat)
{
    tint len = 160 * 2 * 3;
    tbyte *mem = new tbyte [len];
    AudioHardwareBufferALSA buffer(SND_PCM_FORMAT_S18_3BE,160,2,mem,len);
    EXPECT_TRUE(buffer.buffer(0)!=0);
    EXPECT_TRUE(buffer.buffer(1)==0);
    EXPECT_EQ(3,buffer.sampleSize(0));
    EXPECT_EQ(2,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(0,buffer.numberOfChannelsInBuffer(1));
    EXPECT_EQ(160,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
    EXPECT_EQ(160 * 2 * 3,buffer.numberOfBytesInBuffer());
    delete [] mem;
}

//-------------------------------------------------------------------------------------------
