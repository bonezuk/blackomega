#include "gtest/gtest.h"

#include "audioio/inc/AOQuerySharedDevice.h"

using namespace omega;
using namespace audioio;

//-------------------------------------------------------------------------------------------

bool testExpectedFrequenciesIsInArray(int testFreq, const int *expect, int eLen)
{
    bool res = false;
    for(tint i = 0; i < eLen && !res; i++)
    {
        if(testFreq == expect[i])
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void testExpectedFrequenciesOfSharedDevice(AOQuerySharedDevice& device, const int *expectFreqs, int expectLen)
{
    const int fullFreqs[18] = {
        768000, 705600, 384000, 352800, // 4
        192000, 176400, 96000, // 7
         88200,  64000, 48000, // 10
         44100,  32000, 24000, // 13
         22050,  16000, 12000, // 16
         11025,   8000 // 18
    };

    for(int i = 0; i < 18; i++)
    {
        int testFreq = fullFreqs[i];

        if(testExpectedFrequenciesIsInArray(testFreq, expectFreqs, expectLen))
        {
            EXPECT_TRUE(device.isFrequencySupported(testFreq));
            EXPECT_TRUE(device.frequencies().find(testFreq) != device.frequencies().end());
        }
        else
        {
            EXPECT_FALSE(device.isFrequencySupported(testFreq));
            EXPECT_TRUE(device.frequencies().find(testFreq) == device.frequencies().end());
        }
    }

}

//-------------------------------------------------------------------------------------------

TEST(AOQuerySharedDevice, frequency)
{
    const tint c_freqExclusive[3] = { 44100, 176400, 192000 };
    const tint c_freqShared[2] = { 44100, 48000 };

    tint i;
    AOQuerySharedDevice device;
    device.name() = "test";
    EXPECT_EQ(device.name(), "test");
    
    device.setAccessMode(e_Exclusive);
    EXPECT_EQ(device.accessMode(), e_Exclusive);
    for(i = 0; i < 3; i++)
    {
        device.addFrequency(c_freqExclusive[i]);
    }
    device.setAccessMode(e_Shared);
    EXPECT_EQ(device.accessMode(), e_Shared);
    for(i = 0; i < 3; i++)
    {
        device.addFrequency(c_freqShared[i]);
    }
    
    AudioSettings::instance(device.name())->setExclusive(true);
    device.setAccessMode(e_Settings);
    EXPECT_EQ(device.accessMode(), e_Settings);
    testExpectedFrequenciesOfSharedDevice(device, c_freqExclusive, 3);
    
    device.setAccessMode(e_Shared);
    testExpectedFrequenciesOfSharedDevice(device, c_freqShared, 2);
    
    device.setAccessMode(e_Settings);
    EXPECT_EQ(device.accessMode(), e_Settings);
    AudioSettings::instance(device.name())->setExclusive(false);
    testExpectedFrequenciesOfSharedDevice(device, c_freqShared, 2);

    device.setAccessMode(e_Exclusive);
    testExpectedFrequenciesOfSharedDevice(device, c_freqExclusive, 3);
}

//-------------------------------------------------------------------------------------------

TEST(AOQuerySharedDevice, channelMapDefault)
{
    AOChannelMap *map;
    AOQuerySharedDevice device;
    device.name() = "test";
    EXPECT_EQ(device.name(), "test");

    device.setAccessMode(e_Exclusive);
    device.setNoChannels(8);
    
    device.setAccessMode(e_Shared);
    device.setNoChannels(2);
    
    AudioSettings::instance(device.name())->setExclusive(false);
    device.setAccessMode(e_Settings);
    EXPECT_EQ(device.accessMode(), e_Settings);
    
    EXPECT_EQ(device.noChannels(), 2);
    map = device.channelMap();
    EXPECT_EQ(map->noMappedChannels(), 2);
    EXPECT_EQ(map->noDeviceChannels(), 2);
    EXPECT_EQ(map->stereoType(), AOChannelMap::e_Front);
    EXPECT_EQ(map->channel(e_FrontLeft), 0);
    EXPECT_EQ(map->channel(e_FrontRight), 1);
    EXPECT_EQ(map->channel(e_Center), -1);
    EXPECT_EQ(map->channel(e_LFE), -1);
    EXPECT_EQ(map->channel(e_SurroundLeft), -1);
    EXPECT_EQ(map->channel(e_SurroundRight), -1);
    EXPECT_EQ(map->channel(e_RearLeft), -1);
    EXPECT_EQ(map->channel(e_RearRight), -1);
    
    AudioSettings::instance(device.name())->setExclusive(true);
    EXPECT_EQ(device.noChannels(), 8);
    map = device.channelMap();

    EXPECT_EQ(map->noMappedChannels(), 8);
    EXPECT_EQ(map->noDeviceChannels(), 8);
    EXPECT_EQ(map->channel(e_FrontLeft), 0);
    EXPECT_EQ(map->channel(e_FrontRight), 1);
    EXPECT_EQ(map->channel(e_Center), 2);
    EXPECT_EQ(map->channel(e_LFE), 3);
    EXPECT_EQ(map->channel(e_SurroundLeft), 6);
    EXPECT_EQ(map->channel(e_SurroundRight), 7);
    EXPECT_EQ(map->channel(e_RearLeft), 4);
    EXPECT_EQ(map->channel(e_RearRight), 5);
}

//-------------------------------------------------------------------------------------------

void removeOldAudioChannelSettingsSharedDevice(const QString& devName)
{
    QSettings settings;
    QString groupName = "audio" + devName;
    settings.remove(groupName);
    for(int i = 0; i <= static_cast<ChannelType>(e_UnknownChannel); i++)
    {
        QString name = groupName + "_" + QString::number(i);
        settings.remove(name);
    }
}

//-------------------------------------------------------------------------------------------

TEST(AOQuerySharedDevice, modifyLoadAndSaveChannelMaps)
{
    AOChannelMap *map;
    
    {
        AOQuerySharedDevice device;
        device.name() = "test";
        EXPECT_EQ(device.name(), "test");
    
        removeOldAudioChannelSettingsSharedDevice(device.name());
        
        device.setAccessMode(e_Exclusive);
        map = device.channelMap();
        device.setNoChannels(8);
        map->setChannel(e_FrontLeft, 1);
        map->setChannel(e_SurroundLeft, 7);
        map->setChannel(e_RearLeft, 5);
        map->setStereoType(AOChannelMap::e_Surround);
        
        EXPECT_EQ(device.noChannels(), 8);
        EXPECT_EQ(device.channelMap()->noMappedChannels(), 8);
        EXPECT_EQ(device.channelMap()->noDeviceChannels(), 8);
        EXPECT_EQ(map->channel(e_FrontLeft), 1);
        EXPECT_EQ(map->channel(e_FrontRight), 0);
        EXPECT_EQ(map->channel(e_Center), 2);
        EXPECT_EQ(map->channel(e_LFE), 3);
        EXPECT_EQ(map->channel(e_SurroundLeft), 7);
        EXPECT_EQ(map->channel(e_SurroundRight), 6);
        EXPECT_EQ(map->channel(e_RearLeft), 5);
        EXPECT_EQ(map->channel(e_RearRight), 4);
        EXPECT_EQ(map->stereoType(), AOChannelMap::e_Surround);
        
        device.setAccessMode(e_Shared);
        device.setNoChannels(2);
        map = device.channelMap();
        map->setChannel(e_FrontLeft, 1);
        
        EXPECT_EQ(device.noChannels(), 2);
        EXPECT_EQ(device.channelMap()->noMappedChannels(), 2);
        EXPECT_EQ(device.channelMap()->noDeviceChannels(), 2);
        EXPECT_EQ(map->channel(e_FrontLeft), 1);
        EXPECT_EQ(map->channel(e_FrontRight), 0);
        EXPECT_EQ(map->channel(e_Center), -1);
        EXPECT_EQ(map->channel(e_LFE), -1);
        EXPECT_EQ(map->channel(e_SurroundLeft), -1);
        EXPECT_EQ(map->channel(e_SurroundRight), -1);
        EXPECT_EQ(map->channel(e_RearLeft), -1);
        EXPECT_EQ(map->channel(e_RearRight), -1);
        EXPECT_EQ(map->stereoType(), AOChannelMap::e_Front);
        
        device.saveChannelMap();        
    }

    {
        AOQuerySharedDevice device;
        device.name() = "test";
        EXPECT_EQ(device.name(), "test");
        device.setAccessMode(e_Exclusive);
        device.setNoChannels(8);
        device.setAccessMode(e_Shared);
        device.setNoChannels(2);
        device.setAccessMode(e_Settings);
        
        device.loadChannelMap();
        
        AudioSettings::instance(device.name())->setExclusive(true);
        map = device.channelMap();

        EXPECT_EQ(device.noChannels(), 8);
        EXPECT_EQ(device.channelMap()->noMappedChannels(), 8);
        EXPECT_EQ(device.channelMap()->noDeviceChannels(), 8);
        EXPECT_EQ(map->channel(e_FrontLeft), 1);
        EXPECT_EQ(map->channel(e_FrontRight), 0);
        EXPECT_EQ(map->channel(e_Center), 2);
        EXPECT_EQ(map->channel(e_LFE), 3);
        EXPECT_EQ(map->channel(e_SurroundLeft), 7);
        EXPECT_EQ(map->channel(e_SurroundRight), 6);
        EXPECT_EQ(map->channel(e_RearLeft), 5);
        EXPECT_EQ(map->channel(e_RearRight), 4);
        EXPECT_EQ(map->stereoType(), AOChannelMap::e_Surround);
        
        AudioSettings::instance(device.name())->setExclusive(false);
        map = device.channelMap();

        EXPECT_EQ(device.noChannels(), 2);
        EXPECT_EQ(device.channelMap()->noMappedChannels(), 2);
        EXPECT_EQ(device.channelMap()->noDeviceChannels(), 2);
        EXPECT_EQ(map->channel(e_FrontLeft), 1);
        EXPECT_EQ(map->channel(e_FrontRight), 0);
        EXPECT_EQ(map->channel(e_Center), -1);
        EXPECT_EQ(map->channel(e_LFE), -1);
        EXPECT_EQ(map->channel(e_SurroundLeft), -1);
        EXPECT_EQ(map->channel(e_SurroundRight), -1);
        EXPECT_EQ(map->channel(e_RearLeft), -1);
        EXPECT_EQ(map->channel(e_RearRight), -1);
        EXPECT_EQ(map->stereoType(), AOChannelMap::e_Front);
        
        removeOldAudioChannelSettingsSharedDevice(device.name());
    }
}

//-------------------------------------------------------------------------------------------

TEST(AOQuerySharedDevice, copyDevice)
{
    const tint c_freqExclusive[3] = { 44100, 176400, 192000 };
    const tint c_freqShared[2] = { 44100, 48000 };

    tint i;
    AOChannelMap *map;
    AOQuerySharedDevice deviceA;
    deviceA.name() = "test";
    EXPECT_EQ(deviceA.name(), "test");

    removeOldAudioChannelSettingsSharedDevice(deviceA.name());
    
    deviceA.setAccessMode(e_Exclusive);
    map = deviceA.channelMap();
    deviceA.setNoChannels(8);
    map->setChannel(e_FrontLeft, 1);
    map->setChannel(e_SurroundLeft, 7);
    map->setChannel(e_RearLeft, 5);
    map->setStereoType(AOChannelMap::e_Surround);

    deviceA.setAccessMode(e_Shared);
    map = deviceA.channelMap();
    deviceA.setNoChannels(2);
    map->setChannel(e_FrontLeft, 1);

    deviceA.setAccessMode(e_Exclusive);
    for(i = 0; i < 3; i++)
    {
        deviceA.addFrequency(c_freqExclusive[i]);
    }
    deviceA.setAccessMode(e_Shared);
    for(i = 0; i < 3; i++)
    {
        deviceA.addFrequency(c_freqShared[i]);
    }

    AOQuerySharedDevice deviceB(deviceA);

    deviceB.setAccessMode(e_Exclusive);
    map = deviceB.channelMap();
    EXPECT_EQ(deviceB.noChannels(), 8);
    EXPECT_EQ(deviceB.channelMap()->noMappedChannels(), 8);
    EXPECT_EQ(deviceB.channelMap()->noDeviceChannels(), 8);
    EXPECT_EQ(map->channel(e_FrontLeft), 1);
    EXPECT_EQ(map->channel(e_FrontRight), 0);
    EXPECT_EQ(map->channel(e_Center), 2);
    EXPECT_EQ(map->channel(e_LFE), 3);
    EXPECT_EQ(map->channel(e_SurroundLeft), 7);
    EXPECT_EQ(map->channel(e_SurroundRight), 6);
    EXPECT_EQ(map->channel(e_RearLeft), 5);
    EXPECT_EQ(map->channel(e_RearRight), 4);
    EXPECT_EQ(map->stereoType(), AOChannelMap::e_Surround);
    testExpectedFrequenciesOfSharedDevice(deviceB, c_freqExclusive, 3);

    deviceB.setAccessMode(e_Shared);
    map = deviceB.channelMap();    
    EXPECT_EQ(deviceB.noChannels(), 2);
    EXPECT_EQ(deviceB.channelMap()->noMappedChannels(), 2);
    EXPECT_EQ(deviceB.channelMap()->noDeviceChannels(), 2);
    EXPECT_EQ(map->channel(e_FrontLeft), 1);
    EXPECT_EQ(map->channel(e_FrontRight), 0);
    EXPECT_EQ(map->channel(e_Center), -1);
    EXPECT_EQ(map->channel(e_LFE), -1);
    EXPECT_EQ(map->channel(e_SurroundLeft), -1);
    EXPECT_EQ(map->channel(e_SurroundRight), -1);
    EXPECT_EQ(map->channel(e_RearLeft), -1);
    EXPECT_EQ(map->channel(e_RearRight), -1);
    EXPECT_EQ(map->stereoType(), AOChannelMap::e_Front);
    testExpectedFrequenciesOfSharedDevice(deviceB, c_freqShared, 2);
}

//-------------------------------------------------------------------------------------------
