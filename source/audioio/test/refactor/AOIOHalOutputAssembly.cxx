
bool AOCoreAudio::isConvertionAlignedHigh(const AudioStreamBasicDescription& format) const
{
    bool alignHigh = false;
    tint packedBytesPerFrame = (format.mBitsPerChannel >> 3) * format.mChannelsPerFrame;
    
    if(packedBytesPerFrame < format.mBytesPerFrame)
    {
        if(format.mFormatFlags & kAudioFormatFlagIsBigEndian)
        {
            alignHigh = (format.mFormatFlags & kAudioFormatFlagIsAlignedHigh) ? true : false;
        }
        else
        {
            alignHigh = (format.mFormatFlags & kAudioFormatFlagIsAlignedHigh) ? false : true;
        }
    }
    return alignHigh;
}

class AOCoreAudioIsConvertionAlignedHigh : public AOCoreAudio
{
    public:
        virtual bool isConvertionAlignedHigh(const AudioStreamBasicDescription& format) const;
}

bool AOCoreAudioIsConvertionAlignedHigh::isConvertionAlignedHigh(const AudioStreamBasicDescription& format) const
{
    return AOCoreAudio::isConvertionAlignedHigh(format);
}


TEST(AOCoreAudio,isConvertionAlignedHighFor16BitSampleInLittleEndian24BitsAndHighFlagNotSet)
{
    const tint c_bitsPerSample = 16;
    const tint c_noChannels = 2;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 3;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_TRUE(audio.isConvertionAlignedHigh(format));
}

TEST(AOCoreAudio,isConvertionAlignedHighFor16BitSampleInLittleEndian24BitsAndHighFlagIsSet)
{
    const tint c_bitsPerSample = 16;
    const tint c_noChannels = 2;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 3;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsAlignedHigh;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

TEST(AOCoreAudio,isConvertionAlignedHighFor16BitSampleInBigEndian24BitsAndHighFlagNotSet)
{
    const tint c_bitsPerSample = 16;
    const tint c_noChannels = 2;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 3;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

TEST(AOCoreAudio,isConvertionAlignedHighFor16BitSampleInBigEndian24BitsAndHighFlagIsSet)
{
    const tint c_bitsPerSample = 16;
    const tint c_noChannels = 2;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 3;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsAlignedHigh;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

TEST(AOCoreAudio,isConvertionAlignedHighFor16BitSampleInLittleEndian32BitsAndHighFlagNotSet)
{
    const tint c_bitsPerSample = 16;
    const tint c_noChannels = 2;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 4;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_TRUE(audio.isConvertionAlignedHigh(format));
}

TEST(AOCoreAudio,isConvertionAlignedHighFor16BitSampleInLittleEndian32BitsAndHighFlagIsSet)
{
    const tint c_bitsPerSample = 16;
    const tint c_noChannels = 2;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 4;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsAlignedHigh;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

TEST(AOCoreAudio,isConvertionAlignedHighFor16BitSampleInBigEndian32BitsAndHighFlagNotSet)
{
    const tint c_bitsPerSample = 16;
    const tint c_noChannels = 8;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 4;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

TEST(AOCoreAudio,isConvertionAlignedHighFor16BitSampleInBigEndian32BitsAndHighFlagIsSet)
{
    const tint c_bitsPerSample = 16;
    const tint c_noChannels = 4;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 4;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsAlignedHigh;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_TRUE(audio.isConvertionAlignedHigh(format));
}

TEST(AOCoreAudio,isConvertionAlignedHighFor24BitSampleInLittleEndian32BitsAndHighFlagNotSet)
{
    const tint c_bitsPerSample = 24;
    const tint c_noChannels = 4;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 4;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_TRUE(audio.isConvertionAlignedHigh(format));
}

TEST(AOCoreAudio,isConvertionAlignedHighFor24BitSampleInLittleEndian32BitsAndHighFlagIsSet)
{
    const tint c_bitsPerSample = 24;
    const tint c_noChannels = 4;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 4;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsAlignedHigh;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

TEST(AOCoreAudio,isConvertionAlignedHighFor24BitSampleInBigEndian32BitsAndHighFlagNotSet)
{
    const tint c_bitsPerSample = 24;
    const tint c_noChannels = 2;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 4;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

TEST(AOCoreAudio,isConvertionAlignedHighFor24BitSampleInBigEndian32BitsAndHighFlagIsSet)
{
    const tint c_bitsPerSample = 24;
    const tint c_noChannels = 2;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 4;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsAlignedHigh;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_TRUE(audio.isConvertionAlignedHigh(format));
}

TEST(AOCoreAudio,isConvertionAlignedHighFor32BitSampleInLittleEndian32BitsAndHighFlagNotSet)
{
    const tint c_bitsPerSample = 32;
    const tint c_noChannels = 2;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 4;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsAlignedHigh;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

TEST(AOCoreAudio,isConvertionAlignedHighFor32BitSampleInLittleEndian32BitsAndHighFlagIsSet)
{
    const tint c_bitsPerSample = 32;
    const tint c_noChannels = 4;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 4;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger |  kAudioFormatFlagIsAlignedHigh;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

TEST(AOCoreAudio,isConvertionAlignedHighFor32BitSampleInBigEndian32BitsAndHighFlagNotSet)
{
    const tint c_bitsPerSample = 32;
    const tint c_noChannels = 2;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 4;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

TEST(AOCoreAudio,isConvertionAlignedHighFor32BitSampleInBigEndian32BitsAndHighFlagIsSet)
{
    const tint c_bitsPerSample = 32;
    const tint c_noChannels = 2;
    const tint c_noFramesPerPacket = 1;
    const tint c_bytesPerSample = 4;

    AudioStreamBasicDescription format;
    format.mSampleRate = 44100.0;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsAlignedHigh;
    format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
    format.mFramesPerPacket = c_noFramesPerPacket;
    format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
    format.mChannelsPerFrame = c_noChannels;
    format.mBitsPerChannel = c_bitsPerSample;
    format.mReserved = 0;

    AOCoreAudioIsConvertionAlignedHigh audio;
    
    EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}




// kAudioFormatFlagIsAlignedHigh - IS set
TEST(SampleConverter,doubleSamplesTo16BitSampleInLittleEndian24BitsLSB)
{
    const tint c_noBits = 16;
    const tint c_bytesPerSample = 3;
    const bool c_littleEndian = true;
    const bool c_alignHigh = false;

    const tfloat64 c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
    
    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0xff, 0x7f, 0x00,
        0x66, 0x66, 0x00,
        0xcc, 0x4c, 0x00,
        0x33, 0x33, 0x00,
        0x99, 0x19, 0x00,
        0x66, 0xe6, 0xff,
        0xcd, 0xcc, 0xff,
        0x33, 0xb3, 0xff,
        0x9a, 0x99, 0xff,
        0x00, 0x80, 0xff,
        0xff, 0x7f, 0x00,
        0x00, 0x80, 0xff
    };
    
    testSampleConvertion(c_noBits,c_bytesPerSample,c_littleEndian,c_alignHigh,c_sampleInput,c_expectOutput);
}

// kAudioFormatFlagIsAlignedHigh - NOT set
TEST(SampleConverter,doubleSamplesTo16BitSampleInLittleEndian24BitsMSB)
{
    const tint c_noBits = 16;
    const tint c_bytesPerSample = 3;
    const bool c_littleEndian = true;
    const bool c_alignHigh = true;

    const tfloat64 c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
    
    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0x00, 0xff, 0x7f,
        0x00, 0x66, 0x66,
        0x00, 0xcc, 0x4c,
        0x00, 0x33, 0x33,
        0x00, 0x99, 0x19,
        0x00, 0x66, 0xe6,
        0x00, 0xcd, 0xcc,
        0x00, 0x33, 0xb3,
        0x00, 0x9a, 0x99,
        0x00, 0x00, 0x80,
        0x00, 0xff, 0x7f,
        0x00, 0x00, 0x80
    };
    
    testSampleConvertion(c_noBits,c_bytesPerSample,c_littleEndian,c_alignHigh,c_sampleInput,c_expectOutput);
}

// kAudioFormatFlagIsAlignedHigh - NOT set
TEST(SampleConverter,doubleSamplesTo16BitSampleInBigEndian24BitsLSB)
{
    const tint c_noBits = 16;
    const tint c_bytesPerSample = 3;
    const bool c_littleEndian = false;
    const bool c_alignHigh = false;

    const tfloat64 c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
    
    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0x00, 0x7f, 0xff,
        0x00, 0x66, 0x66,
        0x00, 0x4c, 0xcc,
        0x00, 0x33, 0x33,
        0x00, 0x19, 0x99,
        0xff, 0xe6, 0x66,
        0xff, 0xcc, 0xcd,
        0xff, 0xb3, 0x33,
        0xff, 0x99, 0x9a,
        0xff, 0x80, 0x00,
        0x00, 0x7f, 0xff,
        0xff, 0x80, 0x00
    };
    
    testSampleConvertion(c_noBits,c_bytesPerSample,c_littleEndian,c_alignHigh,c_sampleInput,c_expectOutput);
}

// kAudioFormatFlagIsAlignedHigh - IS set
TEST(SampleConverter,doubleSamplesTo16BitSampleInBigEndian24BitsMSB)
{
    const tint c_noBits = 16;
    const tint c_bytesPerSample = 3;
    const bool c_littleEndian = false;
    const bool c_alignHigh = true;

    const tfloat64 c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
    
    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0x7f, 0xff, 0x00,
        0x66, 0x66, 0x00,
        0x4c, 0xcc, 0x00,
        0x33, 0x33, 0x00,
        0x19, 0x99, 0x00,
        0xe6, 0x66, 0x00,
        0xcc, 0xcd, 0x00,
        0xb3, 0x33, 0x00,
        0x99, 0x9a, 0x00,
        0x80, 0x00, 0x00,
        0x7f, 0xff, 0x00,
        0x80, 0x00, 0x00
    };
    
    testSampleConvertion(c_noBits,c_bytesPerSample,c_littleEndian,c_alignHigh,c_sampleInput,c_expectOutput);
}

// kAudioFormatFlagIsAlignedHigh - IS set
TEST(SampleConverter,doubleSamplesTo16BitSampleInLittleEndian32BitsLSB)
{
    const tint c_noBits = 16;
    const tint c_bytesPerSample = 4;
    const bool c_littleEndian = true;
    const bool c_alignHigh = false;

    const tfloat64 c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
    
    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0xff, 0x7f, 0x00, 0x00,
        0x66, 0x66, 0x00, 0x00,
        0xcc, 0x4c, 0x00, 0x00,
        0x33, 0x33, 0x00, 0x00,
        0x99, 0x19, 0x00, 0x00,
        0x66, 0xe6, 0xff, 0xff,
        0xcd, 0xcc, 0xff, 0xff,
        0x33, 0xb3, 0xff, 0xff,
        0x9a, 0x99, 0xff, 0xff,
        0x00, 0x80, 0xff, 0xff,
        0xff, 0x7f, 0x00, 0x00,
        0x00, 0x80, 0xff, 0xff
    };
    
    testSampleConvertion(c_noBits,c_bytesPerSample,c_littleEndian,c_alignHigh,c_sampleInput,c_expectOutput);
}

// kAudioFormatFlagIsAlignedHigh - NOT set
TEST(SampleConverter,doubleSamplesTo16BitSampleInLittleEndian32BitsMSB)
{
    const tint c_noBits = 16;
    const tint c_bytesPerSample = 4;
    const bool c_littleEndian = true;
    const bool c_alignHigh = true;

    const tfloat64 c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
    
    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0x00, 0x00, 0xff, 0x7f,
        0x00, 0x00, 0x66, 0x66,
        0x00, 0x00, 0xcc, 0x4c,
        0x00, 0x00, 0x33, 0x33,
        0x00, 0x00, 0x99, 0x19,
        0x00, 0x00, 0x66, 0xe6,
        0x00, 0x00, 0xcd, 0xcc,
        0x00, 0x00, 0x33, 0xb3,
        0x00, 0x00, 0x9a, 0x99,
        0x00, 0x00, 0x00, 0x80,
        0x00, 0x00, 0xff, 0x7f,
        0x00, 0x00, 0x00, 0x80
    };
    
    testSampleConvertion(c_noBits,c_bytesPerSample,c_littleEndian,c_alignHigh,c_sampleInput,c_expectOutput);
}

// kAudioFormatFlagIsAlignedHigh - NOT set
TEST(SampleConverter,doubleSamplesTo16BitSampleInBigEndian32BitsLSB)
{
    const tint c_noBits = 16;
    const tint c_bytesPerSample = 4;
    const bool c_littleEndian = false;
    const bool c_alignHigh = false;

    const tfloat64 c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
    
    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
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
    
    testSampleConvertion(c_noBits,c_bytesPerSample,c_littleEndian,c_alignHigh,c_sampleInput,c_expectOutput);
}


// kAudioFormatFlagIsAlignedHigh - IS set
TEST(SampleConverter,doubleSamplesTo16BitSampleInBigEndian32BitsMSB)
{
    const tint c_noBits = 16;
    const tint c_bytesPerSample = 4;
    const bool c_littleEndian = false;
    const bool c_alignHigh = true;

    const tfloat64 c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
    
    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0x7f, 0xff, 0x00, 0x00,
        0x66, 0x66, 0x00, 0x00,
        0x4c, 0xcc, 0x00, 0x00,
        0x33, 0x33, 0x00, 0x00,
        0x19, 0x99, 0x00, 0x00,
        0xe6, 0x66, 0x00, 0x00,
        0xcc, 0xcd, 0x00, 0x00,
        0xb3, 0x33, 0x00, 0x00,
        0x99, 0x9a, 0x00, 0x00,
        0x80, 0x00, 0x00, 0x00,
        0x7f, 0xff, 0x00, 0x00,
        0x80, 0x00, 0x00, 0x00
    };
    
    testSampleConvertion(c_noBits,c_bytesPerSample,c_littleEndian,c_alignHigh,c_sampleInput,c_expectOutput);
}

// kAudioFormatFlagIsAlignedHigh - IS set
TEST(SampleConverter,doubleSamplesTo24BitSampleInLittleEndian32BitsLSB)
{
    const tint c_noBits = 24;
    const tint c_bytesPerSample = 4;
    const bool c_littleEndian = true;
    const bool c_alignHigh = false;

    const tfloat64 c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
    
    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0xff, 0xff, 0x7f, 0x00,
        0x66, 0x66, 0x66, 0x00,
        0xcc, 0xcc, 0x4c, 0x00,
        0x33, 0x33, 0x33, 0x00,
        0x99, 0x99, 0x19, 0x00,
        0x66, 0x66, 0xe6, 0xff,
        0xcd, 0xcc, 0xcc, 0xff,
        0x33, 0x33, 0xb3, 0xff,
        0x9a, 0x99, 0x99, 0xff,
        0x00, 0x00, 0x80, 0xff,
        0xff, 0xff, 0x7f, 0x00,
        0x00, 0x00, 0x80, 0xff
    };
    
    testSampleConvertion(c_noBits,c_bytesPerSample,c_littleEndian,c_alignHigh,c_sampleInput,c_expectOutput);
}

// kAudioFormatFlagIsAlignedHigh - NOT set
TEST(SampleConverter,doubleSamplesTo24BitSampleInLittleEndian32BitsMSB)
{
    const tint c_noBits = 24;
    const tint c_bytesPerSample = 4;
    const bool c_littleEndian = true;
    const bool c_alignHigh = true;

    const tfloat64 c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
    
    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0x00, 0xff, 0xff, 0x7f,
        0x00, 0x66, 0x66, 0x66,
        0x00, 0xcc, 0xcc, 0x4c,
        0x00, 0x33, 0x33, 0x33,
        0x00, 0x99, 0x99, 0x19,
        0x00, 0x66, 0x66, 0xe6,
        0x00, 0xcd, 0xcc, 0xcc,
        0x00, 0x33, 0x33, 0xb3,
        0x00, 0x9a, 0x99, 0x99,
        0x00, 0x00, 0x00, 0x80,
        0x00, 0xff, 0xff, 0x7f,
        0x00, 0x00, 0x00, 0x80
    };
    
    testSampleConvertion(c_noBits,c_bytesPerSample,c_littleEndian,c_alignHigh,c_sampleInput,c_expectOutput);
}

// kAudioFormatFlagIsAlignedHigh - NOT set
TEST(SampleConverter,doubleSamplesTo24BitSampleInBigEndian32BitsLSB)
{
    const tint c_noBits = 24;
    const tint c_bytesPerSample = 4;
    const bool c_littleEndian = false;
    const bool c_alignHigh = false;

    const tfloat64 c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
    
    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0x00, 0x7f, 0xff, 0xff,
        0x00, 0x66, 0x66, 0x66,
        0x00, 0x4c, 0xcc, 0xcc,
        0x00, 0x33, 0x33, 0x33,
        0x00, 0x19, 0x99, 0x99,
        0xff, 0xe6, 0x66, 0x66,
        0xff, 0xcc, 0xcc, 0xcd,
        0xff, 0xb3, 0x33, 0x33,
        0xff, 0x99, 0x99, 0x9a,
        0xff, 0x80, 0x00, 0x00,
        0x00, 0x7f, 0xff, 0xff,
        0xff, 0x80, 0x00, 0x00
    };
    
    testSampleConvertion(c_noBits,c_bytesPerSample,c_littleEndian,c_alignHigh,c_sampleInput,c_expectOutput);
}

// kAudioFormatFlagIsAlignedHigh - IS set
TEST(SampleConverter,doubleSamplesTo24BitSampleInBigEndian32BitsMSB)
{
    const tint c_noBits = 24;
    const tint c_bytesPerSample = 4;
    const bool c_littleEndian = false;
    const bool c_alignHigh = true;

    const tfloat64 c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
    
    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0x7f, 0xff, 0xff, 0x00,
        0x66, 0x66, 0x66, 0x00,
        0x4c, 0xcc, 0xcc, 0x00,
        0x33, 0x33, 0x33, 0x00,
        0x19, 0x99, 0x99, 0x00,
        0xe6, 0x66, 0x66, 0x00,
        0xcc, 0xcc, 0xcd, 0x00,
        0xb3, 0x33, 0x33, 0x00,
        0x99, 0x99, 0x9a, 0x00,
        0x80, 0x00, 0x00, 0x00,
        0x7f, 0xff, 0xff, 0x00,
        0x80, 0x00, 0x00, 0x00
    };
    
    testSampleConvertion(c_noBits,c_bytesPerSample,c_littleEndian,c_alignHigh,c_sampleInput,c_expectOutput);
}

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

AudioStreamBasicDescription *AOCoreAudio::saveAudioDescriptionForStream(AudioStreamID streamID)
{
    OSStatus err;
    UInt32 paramSize = sizeof(AudioStreamBasicDescription);
    AudioObjectPropertyAddress property = { kAudioStreamPropertyPhysicalFormat, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
    AudioStreamBasicDescription *format = new AudioStreamBasicDescription;
    
    memset(format,0,sizeof(AudioStreamBasicDescription));
    
    err = CoreAudioIF::instance()->AudioObjectGetPropertyData(streamID,&property,0,0,&paramSize,format);
    if(err!=noErr)
    {
        printErrorOS("saveAudioDescriptionForStream","Error getting current audio stream description",err);
        delete format;
        format = 0;
    }
    return format;
}


    AudioObjectPropertyAddress audioDeviceAliveAddress = { kAudioDevicePropertyDeviceIsAlive, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
    if (p_sys->i_selected_dev > 0) {

        /* Check if device is in devices list. Only checking for kAudioDevicePropertyDeviceIsAlive is not sufficient,
         * as a former airplay device might be already gone, but the device number might be still valid. Core Audio
         * even says that this device would be alive. Don't ask why, its Core Audio. */
        CFIndex count = CFArrayGetCount(p_sys->device_list);
        CFNumberRef deviceNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &p_sys->i_selected_dev);
        if (CFArrayContainsValue(p_sys->device_list, CFRangeMake(0, count), deviceNumber)) {

            /* Check if the desired device is alive and usable */
            i_param_size = sizeof(b_alive);
            err = AudioObjectGetPropertyData(p_sys->i_selected_dev, &audioDeviceAliveAddress, 0, NULL, &i_param_size, &b_alive);
            if (err != noErr) {
                /* Be tolerant, only give a warning here */
                msg_Warn(p_aout, "could not check whether device [0x%x] is alive [%4.4s]",
                         (unsigned int)p_sys->i_selected_dev, (char *)&err);
                b_alive = false;
            }

            if (!b_alive)
                msg_Warn(p_aout, "selected audio device is not alive, switching to default device");

        } else {
            msg_Warn(p_aout, "device id %i not found in the current devices list, fallback to default device", p_sys->i_selected_dev);
        }
        CFRelease(deviceNumber);
    }




bool AOCoreAudio::isDeviceAlive(QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio>& pDevice)
{
    OSStatus err;
    bool isAlive = false;
    UInt32 paramSize = sizeof(bool);
    AudioObjectPropertyAddress property = { kAudioDevicePropertyDeviceIsAlive, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
    
    err = CoreAudioIF::instance()->AudioObjectGetPropertyData(pDevice->deviceID(),&property,0,0,&paramSize,&isAlive);
    if(err!=noErr)
    {
        printErrorOS("isDeviceAlive","Failed to check whether device is alive",err);
        isAlive = false;
    }
    
    if(!isAlive)
    {
        QString msg = "Audio device '" + pDevice->name() + "' is no longer alive and available";
        printToLog(msg.toUtf8().constData());
    }
    return isAlive;
}

class AOCoreAudioIsDeviceAliveTest : public AOCoreAudio
{
    public:
        MOCK_CONST_METHOD1(printToLog,void(const tchar *msg));
        MOCK_CONST_METHOD3(printErrorOS,void(const tchar *strR,const tchar *strE,OSStatus err));
        bool testIsDeviceAlive(QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio>& pDevice);
};

bool AOCoreAudioIsDeviceAliveTest::testIsDeviceAlive(QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio>& pDevice)
{
    return isDeviceAlive(pDevice);
}

MATCHER(PropertyIsDeviceAlive, "") { return (arg->mSelector==kAudioDevicePropertyDeviceIsAlive && arg->mScope==kAudioObjectPropertyScopeGlobal && arg->mElement==kAudioObjectPropertyElementMaster); }
ACTION_P(SetIsDeviceAlive,value) { *static_cast<bool*>(arg5) = value; }

TEST(AOCoreAudio,isDeviceAliveGivenNoOSErrorAndDeviceIsAlive)
{
    AudioDeviceID devID = static_cast<AudioDeviceID>(300);
    
    QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pDevice(new AOQueryCoreAudio::DeviceCoreAudio());
    pDevice->setInitialized();
    pDevice->id() = "Device";
    pDevice->name() = "SoundBlaster";
    pDevice->setDeviceID(devID);

    CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
    CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));    
    EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsDeviceAlive(),0,0,PropertyHasSize(sizeof(sizeof(bool))),A<void *>()))
        .Times(1).WillOnce(DoAll(SetIsDeviceAlive(true),Return(static_cast<OSStatus>(noErr))));

    AOCoreAudioIsDeviceAliveTest audio;
    
    EXPECT_TRUE(audio.testIsDeviceAlive(pDevice));

    CoreAudioIF::release();
}

TEST(AOCoreAudio,isDeviceAliveGivenNoOSErrorAndDeviceIsNotAlive)
{
    AudioDeviceID devID = static_cast<AudioDeviceID>(300);
    
    QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pDevice(new AOQueryCoreAudio::DeviceCoreAudio());
    pDevice->setInitialized();
    pDevice->id() = "Device";
    pDevice->name() = "SoundBlaster";
    pDevice->setDeviceID(devID);

    CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
    CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));    
    EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsDeviceAlive(),0,0,PropertyHasSize(sizeof(sizeof(bool))),A<void *>()))
        .Times(1).WillOnce(DoAll(SetIsDeviceAlive(false),Return(static_cast<OSStatus>(noErr))));

    AOCoreAudioIsDeviceAliveTest audio;
    EXPECT_CALL(audio,printToLog(StrEq("Audio device 'SoundBlaster' is no longer alive and available"))).Times(1);
    
    EXPECT_FALSE(audio.testIsDeviceAlive(pDevice));

    CoreAudioIF::release();
}

TEST(AOCoreAudio,isDeviceAliveGivenOSErrorButDeviceIsAliveFlagIsSetToTrue)
{
    AudioDeviceID devID = static_cast<AudioDeviceID>(300);
    
    QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pDevice(new AOQueryCoreAudio::DeviceCoreAudio());
    pDevice->setInitialized();
    pDevice->id() = "Device";
    pDevice->name() = "SoundBlaster";
    pDevice->setDeviceID(devID);

    CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
    CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));    
    EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsDeviceAlive(),0,0,PropertyHasSize(sizeof(sizeof(bool))),A<void *>()))
        .Times(1).WillOnce(DoAll(SetIsDeviceAlive(true),Return(static_cast<OSStatus>(kAudioHardwareNotRunningError))));

    AOCoreAudioIsDeviceAliveTest audio;
    EXPECT_CALL(audio,printErrorOS(StrEq("isDeviceAlive"),StrEq("Failed to check whether device is alive"),Eq(kAudioHardwareNotRunningError))).Times(1);
    EXPECT_CALL(audio,printToLog(StrEq("Audio device 'SoundBlaster' is no longer alive and available"))).Times(1);
    
    EXPECT_FALSE(audio.testIsDeviceAlive(pDevice));

    CoreAudioIF::release();
}
