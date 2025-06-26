#include "gtest/gtest.h"

#include "audioio/inc/AudioHardwareBufferWasAPI.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferWasAPI,bufferWithMono8Bits)
{
    tbyte tBuffer[16];
    WAVEFORMATEX format;

    memset(&format,0,sizeof(WAVEFORMATEX));
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 1;
    format.nSamplesPerSec = 22050;
    format.nAvgBytesPerSec = 22050;
    format.nBlockAlign = 1;
    format.wBitsPerSample = 8;
    format.cbSize = 0;

    AudioHardwareBufferWasAPI buffer(&format,tBuffer,32);

    EXPECT_EQ(tBuffer,buffer.buffer(0));
    EXPECT_EQ(1,buffer.sampleSize(0));
    EXPECT_EQ(1,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(32,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferWasAPI,bufferWithStereo16Bits)
{
    tbyte tBuffer[16];
    WAVEFORMATEX format;

    memset(&format,0,sizeof(WAVEFORMATEX));
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 2;
    format.nSamplesPerSec = 44100;
    format.nAvgBytesPerSec = 44100 * 2 * 2;
    format.nBlockAlign = 2 * 2;
    format.wBitsPerSample = 16;
    format.cbSize = 0;

    AudioHardwareBufferWasAPI buffer(&format,tBuffer,48);

    EXPECT_EQ(tBuffer,buffer.buffer(0));
    EXPECT_EQ(2,buffer.sampleSize(0));
    EXPECT_EQ(2,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(48,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferWasAPI,bufferWithQuadrophonic24BitsAndExtendedFormat)
{
    tbyte tBuffer[16];
    WAVEFORMATEXTENSIBLE pExFormat;

    memset(&pExFormat,0,sizeof(WAVEFORMATEXTENSIBLE));
    pExFormat.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    pExFormat.Format.nChannels = 4;
    pExFormat.Format.nSamplesPerSec = 96000;
    pExFormat.Format.nAvgBytesPerSec = 96000 * 4 * 3;
    pExFormat.Format.nBlockAlign = 4 * 3;
    pExFormat.Format.wBitsPerSample = 24;
    pExFormat.Format.cbSize = 22;
    pExFormat.Samples.wValidBitsPerSample = 20;
    pExFormat.dwChannelMask = KSAUDIO_SPEAKER_QUAD;
    pExFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

    AudioHardwareBufferWasAPI buffer(reinterpret_cast<WAVEFORMATEX *>(&pExFormat),tBuffer,48);

    EXPECT_EQ(tBuffer,buffer.buffer(0));
    EXPECT_EQ(3,buffer.sampleSize(0));
    EXPECT_EQ(4,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(48,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
}

//-------------------------------------------------------------------------------------------

TEST(AudioHardwareBufferWasAPI,bufferWith8Channels32BitFloatAndExtendedFormat)
{
    tbyte tBuffer[16];
    WAVEFORMATEXTENSIBLE pExFormat;

    memset(&pExFormat,0,sizeof(WAVEFORMATEXTENSIBLE));
    pExFormat.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    pExFormat.Format.nChannels = 8;
    pExFormat.Format.nSamplesPerSec = 192000;
    pExFormat.Format.nAvgBytesPerSec = 192000 * 8 * 4;
    pExFormat.Format.nBlockAlign = 8 * 4;
    pExFormat.Format.wBitsPerSample = 32;
    pExFormat.Format.cbSize = 22;
    pExFormat.Samples.wValidBitsPerSample = 32;
    pExFormat.dwChannelMask = KSAUDIO_SPEAKER_QUAD;
    pExFormat.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;

    AudioHardwareBufferWasAPI buffer(reinterpret_cast<WAVEFORMATEX *>(&pExFormat),tBuffer,48);

    EXPECT_EQ(tBuffer,buffer.buffer(0));
    EXPECT_EQ(4,buffer.sampleSize(0));
    EXPECT_EQ(8,buffer.numberOfChannelsInBuffer(0));
    EXPECT_EQ(48,buffer.bufferLength());
    EXPECT_EQ(1,buffer.numberOfBuffers());
}

//-------------------------------------------------------------------------------------------
