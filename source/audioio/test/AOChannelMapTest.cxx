#include "gtest/gtest.h"

#include "audioio/inc/AOQueryDevice.h"

using namespace omega;
using namespace audioio;

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, defaultMapFor1Channel)
{
    AOQueryDevice::Device device;
    device.setNoChannels(1);
    EXPECT_EQ(device.noChannels(), 1);

    AOChannelMap *map = device.channelMap();

    EXPECT_EQ(map->noMappedChannels(), 1);
    EXPECT_EQ(map->noDeviceChannels(), 1);
    EXPECT_EQ(map->stereoType(), AOChannelMap::e_Front);

    EXPECT_EQ(map->channel(e_FrontLeft), -1);
    EXPECT_FALSE(map->isValidChannel(e_FrontLeft));

    EXPECT_EQ(map->channel(e_FrontRight), -1);
    EXPECT_FALSE(map->isValidChannel(e_FrontRight));

    EXPECT_EQ(map->channel(e_Center), 0);
    EXPECT_TRUE(map->isValidChannel(e_Center));

    EXPECT_EQ(map->channel(e_LFE), -1);
    EXPECT_FALSE(map->isValidChannel(e_LFE));

    EXPECT_EQ(map->channel(e_SurroundLeft), -1);
    EXPECT_FALSE(map->isValidChannel(e_SurroundLeft));

    EXPECT_EQ(map->channel(e_SurroundRight), -1);
    EXPECT_FALSE(map->isValidChannel(e_SurroundRight));

    EXPECT_EQ(map->channel(e_RearLeft), -1);
    EXPECT_FALSE(map->isValidChannel(e_RearLeft));

    EXPECT_EQ(map->channel(e_RearRight), -1);
    EXPECT_FALSE(map->isValidChannel(e_RearRight));
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, defaultMapFor2Channels)
{
    AOQueryDevice::Device device;
    device.setNoChannels(2);
    EXPECT_EQ(device.noChannels(), 2);

    AOChannelMap *map = device.channelMap();

    EXPECT_EQ(map->noMappedChannels(), 2);
    EXPECT_EQ(map->noDeviceChannels(), 2);
    EXPECT_EQ(map->stereoType(), AOChannelMap::e_Front);

    EXPECT_EQ(map->channel(e_FrontLeft), 0);
    EXPECT_TRUE(map->isValidChannel(e_FrontLeft));

    EXPECT_EQ(map->channel(e_FrontRight), 1);
    EXPECT_TRUE(map->isValidChannel(e_FrontRight));

    EXPECT_EQ(map->channel(e_Center), -1);
    EXPECT_FALSE(map->isValidChannel(e_Center));

    EXPECT_EQ(map->channel(e_LFE), -1);
    EXPECT_FALSE(map->isValidChannel(e_LFE));

    EXPECT_EQ(map->channel(e_SurroundLeft), -1);
    EXPECT_FALSE(map->isValidChannel(e_SurroundLeft));

    EXPECT_EQ(map->channel(e_SurroundRight), -1);
    EXPECT_FALSE(map->isValidChannel(e_SurroundRight));

    EXPECT_EQ(map->channel(e_RearLeft), -1);
    EXPECT_FALSE(map->isValidChannel(e_RearLeft));

    EXPECT_EQ(map->channel(e_RearRight), -1);
    EXPECT_FALSE(map->isValidChannel(e_RearRight));
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, defaultMapFor3Channels)
{
    AOQueryDevice::Device device;
    device.setNoChannels(3);
    EXPECT_EQ(device.noChannels(), 3);

    AOChannelMap *map = device.channelMap();

    EXPECT_EQ(map->noMappedChannels(), 3);
    EXPECT_EQ(map->noDeviceChannels(), 3);
    EXPECT_EQ(map->stereoType(), AOChannelMap::e_Front);

    EXPECT_EQ(map->channel(e_FrontLeft), 0);
    EXPECT_TRUE(map->isValidChannel(e_FrontLeft));

    EXPECT_EQ(map->channel(e_FrontRight), 1);
    EXPECT_TRUE(map->isValidChannel(e_FrontRight));

    EXPECT_EQ(map->channel(e_Center), 2);
    EXPECT_TRUE(map->isValidChannel(e_Center));

    EXPECT_EQ(map->channel(e_LFE), -1);
    EXPECT_FALSE(map->isValidChannel(e_LFE));

    EXPECT_EQ(map->channel(e_SurroundLeft), -1);
    EXPECT_FALSE(map->isValidChannel(e_SurroundLeft));

    EXPECT_EQ(map->channel(e_SurroundRight), -1);
    EXPECT_FALSE(map->isValidChannel(e_SurroundRight));

    EXPECT_EQ(map->channel(e_RearLeft), -1);
    EXPECT_FALSE(map->isValidChannel(e_RearLeft));

    EXPECT_EQ(map->channel(e_RearRight), -1);
    EXPECT_FALSE(map->isValidChannel(e_RearRight));
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, defaultMapFor4Channels)
{
    AOQueryDevice::Device device;
    device.setNoChannels(4);
    EXPECT_EQ(device.noChannels(), 4);

    AOChannelMap *map = device.channelMap();

    EXPECT_EQ(map->noMappedChannels(), 4);
    EXPECT_EQ(map->noDeviceChannels(), 4);
    EXPECT_EQ(map->stereoType(), AOChannelMap::e_FrontRear);

    EXPECT_EQ(map->channel(e_FrontLeft), 0);
    EXPECT_TRUE(map->isValidChannel(e_FrontLeft));

    EXPECT_EQ(map->channel(e_FrontRight), 1);
    EXPECT_TRUE(map->isValidChannel(e_FrontRight));

    EXPECT_EQ(map->channel(e_Center), -1);
    EXPECT_FALSE(map->isValidChannel(e_Center));

    EXPECT_EQ(map->channel(e_LFE), -1);
    EXPECT_FALSE(map->isValidChannel(e_LFE));

    EXPECT_EQ(map->channel(e_SurroundLeft), -1);
    EXPECT_FALSE(map->isValidChannel(e_SurroundLeft));

    EXPECT_EQ(map->channel(e_SurroundRight), -1);
    EXPECT_FALSE(map->isValidChannel(e_SurroundRight));

    EXPECT_EQ(map->channel(e_RearLeft), 2);
    EXPECT_TRUE(map->isValidChannel(e_RearLeft));

    EXPECT_EQ(map->channel(e_RearRight), 3);
    EXPECT_TRUE(map->isValidChannel(e_RearRight));
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, defaultMapFor5Channels)
{
    AOQueryDevice::Device device;
    device.setNoChannels(5);
    EXPECT_EQ(device.noChannels(), 5);

    AOChannelMap *map = device.channelMap();

    EXPECT_EQ(map->noMappedChannels(), 5);
    EXPECT_EQ(map->noDeviceChannels(), 5);
    EXPECT_EQ(map->stereoType(), AOChannelMap::e_FrontRear);

    EXPECT_EQ(map->channel(e_FrontLeft), 0);
    EXPECT_TRUE(map->isValidChannel(e_FrontLeft));

    EXPECT_EQ(map->channel(e_FrontRight), 1);
    EXPECT_TRUE(map->isValidChannel(e_FrontRight));

    EXPECT_EQ(map->channel(e_Center), 2);
    EXPECT_TRUE(map->isValidChannel(e_Center));

    EXPECT_EQ(map->channel(e_LFE), -1);
    EXPECT_FALSE(map->isValidChannel(e_LFE));

    EXPECT_EQ(map->channel(e_SurroundLeft), -1);
    EXPECT_FALSE(map->isValidChannel(e_SurroundLeft));

    EXPECT_EQ(map->channel(e_SurroundRight), -1);
    EXPECT_FALSE(map->isValidChannel(e_SurroundRight));

    EXPECT_EQ(map->channel(e_RearLeft), 3);
    EXPECT_TRUE(map->isValidChannel(e_RearLeft));

    EXPECT_EQ(map->channel(e_RearRight), 4);
    EXPECT_TRUE(map->isValidChannel(e_RearRight));
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, defaultMapFor6Channels)
{
    AOQueryDevice::Device device;
    device.setNoChannels(6);
    EXPECT_EQ(device.noChannels(), 6);

    AOChannelMap *map = device.channelMap();

    EXPECT_EQ(map->noMappedChannels(), 6);
    EXPECT_EQ(map->noDeviceChannels(), 6);
    EXPECT_EQ(map->stereoType(), AOChannelMap::e_FrontRear);

    EXPECT_EQ(map->channel(e_FrontLeft), 0);
    EXPECT_TRUE(map->isValidChannel(e_FrontLeft));

    EXPECT_EQ(map->channel(e_FrontRight), 1);
    EXPECT_TRUE(map->isValidChannel(e_FrontRight));

    EXPECT_EQ(map->channel(e_Center), 2);
    EXPECT_TRUE(map->isValidChannel(e_Center));

    EXPECT_EQ(map->channel(e_LFE), 3);
    EXPECT_TRUE(map->isValidChannel(e_LFE));

    EXPECT_EQ(map->channel(e_SurroundLeft), -1);
    EXPECT_FALSE(map->isValidChannel(e_SurroundLeft));

    EXPECT_EQ(map->channel(e_SurroundRight), -1);
    EXPECT_FALSE(map->isValidChannel(e_SurroundRight));

    EXPECT_EQ(map->channel(e_RearLeft), 4);
    EXPECT_TRUE(map->isValidChannel(e_RearLeft));

    EXPECT_EQ(map->channel(e_RearRight), 5);
    EXPECT_TRUE(map->isValidChannel(e_RearRight));
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, defaultMapFor7Channels)
{
    AOQueryDevice::Device device;
    device.setNoChannels(7);
    EXPECT_EQ(device.noChannels(), 7);

    AOChannelMap *map = device.channelMap();

    EXPECT_EQ(map->noMappedChannels(), 7);
    EXPECT_EQ(map->noDeviceChannels(), 7);
    EXPECT_EQ(map->stereoType(), AOChannelMap::e_FrontSurroundRear);

    EXPECT_EQ(map->channel(e_FrontLeft), 0);
    EXPECT_TRUE(map->isValidChannel(e_FrontLeft));

    EXPECT_EQ(map->channel(e_FrontRight), 1);
    EXPECT_TRUE(map->isValidChannel(e_FrontRight));

    EXPECT_EQ(map->channel(e_Center), 7);
    EXPECT_TRUE(map->isValidChannel(e_Center));

    EXPECT_EQ(map->channel(e_LFE), -1);
    EXPECT_FALSE(map->isValidChannel(e_LFE));

    EXPECT_EQ(map->channel(e_SurroundLeft), 2);
    EXPECT_TRUE(map->isValidChannel(e_SurroundLeft));

    EXPECT_EQ(map->channel(e_SurroundRight), 3);
    EXPECT_TRUE(map->isValidChannel(e_SurroundRight));

    EXPECT_EQ(map->channel(e_RearLeft), 4);
    EXPECT_TRUE(map->isValidChannel(e_RearLeft));

    EXPECT_EQ(map->channel(e_RearRight), 5);
    EXPECT_TRUE(map->isValidChannel(e_RearRight));
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, defaultMapFor8Channels)
{
    AOQueryDevice::Device device;
    device.setNoChannels(8);
    EXPECT_EQ(device.noChannels(), 8);

    AOChannelMap *map = device.channelMap();

    EXPECT_EQ(map->noMappedChannels(), 8);
    EXPECT_EQ(map->noDeviceChannels(), 8);
    EXPECT_EQ(map->stereoType(), AOChannelMap::e_FrontSurroundRear);

    EXPECT_EQ(map->channel(e_FrontLeft), 0);
    EXPECT_TRUE(map->isValidChannel(e_FrontLeft));

    EXPECT_EQ(map->channel(e_FrontRight), 1);
    EXPECT_TRUE(map->isValidChannel(e_FrontRight));

    EXPECT_EQ(map->channel(e_Center), 2);
    EXPECT_TRUE(map->isValidChannel(e_Center));

    EXPECT_EQ(map->channel(e_LFE), 3);
    EXPECT_TRUE(map->isValidChannel(e_LFE));

    EXPECT_EQ(map->channel(e_SurroundLeft), 6);
    EXPECT_TRUE(map->isValidChannel(e_SurroundLeft));

    EXPECT_EQ(map->channel(e_SurroundRight), 7);
    EXPECT_TRUE(map->isValidChannel(e_SurroundRight));

    EXPECT_EQ(map->channel(e_RearLeft), 4);
    EXPECT_TRUE(map->isValidChannel(e_RearLeft));

    EXPECT_EQ(map->channel(e_RearRight), 5);
    EXPECT_TRUE(map->isValidChannel(e_RearRight));
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, setChannelStereoInvalid)
{
    AOQueryDevice::Device device;
    device.setNoChannels(2);
    AOChannelMap *map = device.channelMap();

    EXPECT_FALSE(map->setChannel(e_FrontLeft, 2));
    EXPECT_FALSE(map->setChannel(e_FrontRight, 3));
    EXPECT_FALSE(map->setChannel(e_Center, 0));
    EXPECT_FALSE(map->setChannel(e_LFE, 1));

    EXPECT_EQ(map->channel(e_FrontLeft), 0);
    EXPECT_EQ(map->channel(e_FrontRight), 1);
    EXPECT_EQ(map->noMappedChannels(), 2);
    EXPECT_EQ(map->noDeviceChannels(), 2);
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, setChannelStereoDisableLeft)
{
    AOQueryDevice::Device device;
    device.setNoChannels(2);
    AOChannelMap *map = device.channelMap();

    EXPECT_TRUE(map->setChannel(e_FrontLeft, -1));

    EXPECT_EQ(map->channel(e_FrontLeft), -1);
    EXPECT_EQ(map->channel(e_FrontRight), 1);
    EXPECT_EQ(map->noMappedChannels(), 2);
    EXPECT_EQ(map->noDeviceChannels(), 2);

    EXPECT_TRUE(map->setChannel(e_FrontLeft, 1));

    EXPECT_EQ(map->channel(e_FrontLeft), 1);
    EXPECT_EQ(map->channel(e_FrontRight), 0);
    EXPECT_EQ(map->noMappedChannels(), 2);
    EXPECT_EQ(map->noDeviceChannels(), 2);
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, setChannelStereoDisableRight)
{
    AOQueryDevice::Device device;
    device.setNoChannels(2);
    AOChannelMap *map = device.channelMap();

    EXPECT_TRUE(map->setChannel(e_FrontRight, -1));

    EXPECT_EQ(map->channel(e_FrontLeft), 0);
    EXPECT_EQ(map->channel(e_FrontRight), -1);
    EXPECT_EQ(map->noMappedChannels(), 2);
    EXPECT_EQ(map->noDeviceChannels(), 2);

    EXPECT_TRUE(map->setChannel(e_FrontRight, 0));

    EXPECT_EQ(map->channel(e_FrontLeft), 1);
    EXPECT_EQ(map->channel(e_FrontRight), 0);
    EXPECT_EQ(map->noMappedChannels(), 2);
    EXPECT_EQ(map->noDeviceChannels(), 2);
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, setChannelStereoDisableBoth)
{
    AOQueryDevice::Device device;
    device.setNoChannels(2);
    AOChannelMap *map = device.channelMap();

    EXPECT_TRUE(map->setChannel(e_FrontLeft, -1));
    EXPECT_TRUE(map->setChannel(e_FrontRight, -1));

    EXPECT_EQ(map->channel(e_FrontLeft), -1);
    EXPECT_EQ(map->channel(e_FrontRight), -1);
    EXPECT_EQ(map->noMappedChannels(), 2);
    EXPECT_EQ(map->noDeviceChannels(), 2);

    EXPECT_TRUE(map->setChannel(e_FrontLeft, 0));

    EXPECT_EQ(map->channel(e_FrontLeft), 0);
    EXPECT_EQ(map->channel(e_FrontRight), -1);
    EXPECT_EQ(map->noMappedChannels(), 2);
    EXPECT_EQ(map->noDeviceChannels(), 2);

    EXPECT_TRUE(map->setChannel(e_FrontRight, 1));

    EXPECT_EQ(map->channel(e_FrontLeft), 0);
    EXPECT_EQ(map->channel(e_FrontRight), 1);
    EXPECT_EQ(map->noMappedChannels(), 2);
    EXPECT_EQ(map->noDeviceChannels(), 2);
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, setChannelStereoNoInverse)
{
    AOQueryDevice::Device device;
    device.setNoChannels(2);
    AOChannelMap *map = device.channelMap();

    EXPECT_TRUE(map->setChannel(e_FrontLeft, 0));
    EXPECT_TRUE(map->setChannel(e_FrontRight, 1));

    EXPECT_EQ(map->channel(e_FrontLeft), 0);
    EXPECT_EQ(map->channel(e_FrontRight), 1);
    EXPECT_EQ(map->noMappedChannels(), 2);
    EXPECT_EQ(map->noDeviceChannels(), 2);
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, setChannelStereoInverse)
{
    AOQueryDevice::Device device;
    device.setNoChannels(2);
    AOChannelMap *map = device.channelMap();

    EXPECT_TRUE(map->setChannel(e_FrontLeft, 1));

    EXPECT_EQ(map->channel(e_FrontLeft), 1);
    EXPECT_EQ(map->channel(e_FrontRight), 0);
    EXPECT_EQ(map->noMappedChannels(), 2);
    EXPECT_EQ(map->noDeviceChannels(), 2);
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, setChannelStereoAndCenter)
{
    AOQueryDevice::Device device;
    device.setNoChannels(3);
    AOChannelMap *map = device.channelMap();

    EXPECT_FALSE(map->setChannel(e_LFE, 0));
    EXPECT_FALSE(map->setChannel(e_Center, 3));

    // FL = 0, FR = 1, C = 2
    EXPECT_TRUE(map->setChannel(e_FrontLeft, 2));
    // FL = 2, FR = 1, C = 0
    EXPECT_TRUE(map->setChannel(e_FrontRight, 0));
    // FL = 2, FR = 0, C = 1
    EXPECT_TRUE(map->setChannel(e_Center, 2));
    // FL = 1, FR = 0, C = 2

    EXPECT_EQ(map->channel(e_FrontLeft), 1);
    EXPECT_EQ(map->channel(e_FrontRight), 0);
    EXPECT_EQ(map->channel(e_Center), 2);
    EXPECT_EQ(map->channel(e_LFE), -1);
    EXPECT_EQ(map->channel(e_SurroundLeft), -1);
    EXPECT_EQ(map->channel(e_SurroundRight), -1);
    EXPECT_EQ(map->channel(e_RearLeft), -1);
    EXPECT_EQ(map->channel(e_RearRight), -1);
    EXPECT_EQ(map->noMappedChannels(), 3);
    EXPECT_EQ(map->noDeviceChannels(), 3);
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, setChannelSurroundAndSubwoofer)
{
    AOQueryDevice::Device device;
    device.setNoChannels(6);
    AOChannelMap *map = device.channelMap();

    EXPECT_FALSE(map->setChannel(e_SurroundLeft, 0));
    EXPECT_FALSE(map->setChannel(e_FrontLeft, 6));

    // FL=0, FR=1, C=2, LFE=3, RL=4, RR=5
    EXPECT_TRUE(map->setChannel(e_FrontLeft, 5));
    // FL=5, FR=1, C=2, LFE=3, RL=4, RR=0
    EXPECT_TRUE(map->setChannel(e_RearLeft, 1));
    // FL=5, FR=4, C=2, LFE=3, RL=1, RR=0
    EXPECT_TRUE(map->setChannel(e_LFE, 2));
    // FL=5, FR=4, C=3, LFE=2, RL=1, RR=0

    EXPECT_EQ(map->channel(e_FrontLeft), 5);
    EXPECT_EQ(map->channel(e_FrontRight), 4);
    EXPECT_EQ(map->channel(e_Center), 3);
    EXPECT_EQ(map->channel(e_LFE), 2);
    EXPECT_EQ(map->channel(e_SurroundLeft), -1);
    EXPECT_EQ(map->channel(e_SurroundRight), -1);
    EXPECT_EQ(map->channel(e_RearLeft), 1);
    EXPECT_EQ(map->channel(e_RearRight), 0);
    EXPECT_EQ(map->noMappedChannels(), 6);
    EXPECT_EQ(map->noDeviceChannels(), 6);
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, setFullSurroundToStereoAndInvertChannels)
{
    AOQueryDevice::Device device;
    device.setNoChannels(8);
    AOChannelMap *map = device.channelMap();

    map->setNoMappedChannels(2);

    EXPECT_TRUE(map->setChannel(e_FrontLeft, 1));

    EXPECT_EQ(map->channel(e_FrontLeft), 1);
    EXPECT_EQ(map->channel(e_FrontRight), 0);
    EXPECT_EQ(map->channel(e_Center), -1);
    EXPECT_EQ(map->channel(e_LFE), -1);
    EXPECT_EQ(map->channel(e_SurroundLeft), -1);
    EXPECT_EQ(map->channel(e_SurroundRight), -1);
    EXPECT_EQ(map->channel(e_RearLeft), -1);
    EXPECT_EQ(map->channel(e_RearRight), -1);
    EXPECT_EQ(map->noMappedChannels(), 2);
    EXPECT_EQ(map->noDeviceChannels(), 8);
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, isCenterOnByDefaultWhenAvailable)
{
    AOQueryDevice::Device device;
    device.setNoChannels(3);
    AOChannelMap *map = device.channelMap();
    EXPECT_TRUE(map->isStereoCenter());
}

TEST(AOChannelMap, isCenterOffByDefaultWhenNotAvailable)
{
    AOQueryDevice::Device device;
    device.setNoChannels(2);
    AOChannelMap *map = device.channelMap();
    EXPECT_FALSE(map->isStereoCenter());
    EXPECT_FALSE(map->setStereoCenter(true));
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, isCenterOffWhenSetAndAvailable)
{
    AOQueryDevice::Device device;
    device.setNoChannels(3);
    AOChannelMap *map = device.channelMap();
    EXPECT_TRUE(map->setStereoCenter(false));
    EXPECT_FALSE(map->isStereoCenter());
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, isLFEOnByDefaultWhenAvailable)
{
    AOQueryDevice::Device device;
    device.setNoChannels(8);
    AOChannelMap *map = device.channelMap();
    EXPECT_TRUE(map->isStereoLFE());
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, isLFEOffByDefaultWhenNotAvailable)
{
    AOQueryDevice::Device device;
    device.setNoChannels(2);
    AOChannelMap *map = device.channelMap();
    EXPECT_FALSE(map->isStereoLFE());
    EXPECT_FALSE(map->setStereoLFE(true));
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, isLFEOffWhenSetAndAvailable)
{
    AOQueryDevice::Device device;
    device.setNoChannels(8);
    AOChannelMap *map = device.channelMap();
    EXPECT_TRUE(map->setStereoLFE(false));
    EXPECT_FALSE(map->isStereoLFE());
}

//-------------------------------------------------------------------------------------------

void removeOldAudioChannelSettings(const QString& devName)
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

void testAudioChannelMappingFor2Channels(AOChannelMap *map, int expectLeft, int expectRight)
{
    EXPECT_EQ(map->channel(e_FrontLeft), expectLeft);
    EXPECT_TRUE(map->isValidChannel(e_FrontLeft));

    EXPECT_EQ(map->channel(e_FrontRight), expectRight);
    EXPECT_TRUE(map->isValidChannel(e_FrontRight));

    EXPECT_EQ(map->channel(e_Center), -1);
    EXPECT_FALSE(map->isValidChannel(e_Center));

    EXPECT_EQ(map->channel(e_LFE), -1);
    EXPECT_FALSE(map->isValidChannel(e_LFE));

    EXPECT_EQ(map->channel(e_SurroundLeft), -1);
    EXPECT_FALSE(map->isValidChannel(e_SurroundLeft));

    EXPECT_EQ(map->channel(e_SurroundRight), -1);
    EXPECT_FALSE(map->isValidChannel(e_SurroundRight));

    EXPECT_EQ(map->channel(e_RearLeft), -1);
    EXPECT_FALSE(map->isValidChannel(e_RearLeft));

    EXPECT_EQ(map->channel(e_RearRight), -1);
    EXPECT_FALSE(map->isValidChannel(e_RearRight));
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, saveAndLoadSettingsFor2Channels)
{
    QString devName = "test";
    removeOldAudioChannelSettings(devName);

    {
        AOQueryDevice::Device deviceA;
        deviceA.name() = devName;
        deviceA.setNoChannels(2);

        AOChannelMap *mapA = deviceA.channelMap();
        deviceA.loadChannelMap();
        testAudioChannelMappingFor2Channels(mapA, 0, 1);

        EXPECT_TRUE(mapA->setChannel(e_FrontLeft, 1));
        deviceA.saveChannelMap();
    }

    {
        AOQueryDevice::Device deviceB;
        deviceB.name() = devName;
        deviceB.setNoChannels(2);

        AOChannelMap *mapB = deviceB.channelMap();
        deviceB.loadChannelMap();
        testAudioChannelMappingFor2Channels(mapB, 1, 0);
    }

    removeOldAudioChannelSettings(devName);
}

//-------------------------------------------------------------------------------------------

void testAudioChannelMappingFor8Channels(AOChannelMap *map, int expect[8])
{
    EXPECT_EQ(map->channel(e_FrontLeft), expect[0]);
    EXPECT_TRUE(map->isValidChannel(e_FrontLeft));

    EXPECT_EQ(map->channel(e_FrontRight), expect[1]);
    EXPECT_TRUE(map->isValidChannel(e_FrontRight));

    EXPECT_EQ(map->channel(e_Center), expect[2]);
    EXPECT_TRUE(map->isValidChannel(e_Center));

    EXPECT_EQ(map->channel(e_LFE), expect[3]);
    EXPECT_TRUE(map->isValidChannel(e_LFE));

    EXPECT_EQ(map->channel(e_SurroundLeft), expect[4]);
    EXPECT_TRUE(map->isValidChannel(e_SurroundLeft));

    EXPECT_EQ(map->channel(e_SurroundRight), expect[5]);
    EXPECT_TRUE(map->isValidChannel(e_SurroundRight));

    EXPECT_EQ(map->channel(e_RearLeft), expect[6]);
    EXPECT_TRUE(map->isValidChannel(e_RearLeft));

    EXPECT_EQ(map->channel(e_RearRight), expect[7]);
    EXPECT_TRUE(map->isValidChannel(e_RearRight));
}

//-------------------------------------------------------------------------------------------

void setAudioChannelFor8Channels(AOChannelMap *map, int chIdx[8])
{
    EXPECT_TRUE(map->setChannel(e_FrontLeft, chIdx[0]));
    EXPECT_TRUE(map->setChannel(e_FrontRight, chIdx[1]));
    EXPECT_TRUE(map->setChannel(e_Center, chIdx[2]));
    EXPECT_TRUE(map->setChannel(e_LFE, chIdx[3]));
    EXPECT_TRUE(map->setChannel(e_SurroundLeft, chIdx[4]));
    EXPECT_TRUE(map->setChannel(e_SurroundRight, chIdx[5]));
    EXPECT_TRUE(map->setChannel(e_RearLeft, chIdx[6]));
    EXPECT_TRUE(map->setChannel(e_RearRight, chIdx[7]));
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, saveAndLoadSettingsFor8Channels)
{
    int expectDefault[8] = {  0,  1,  2,  3,  6,  7,  4,  5 };
    int expectLoaded[8] = { 1, 0, 2, 3, 7, 6, 4, 5 };

    QString devName = "test";
    removeOldAudioChannelSettings(devName);

    {
        AOQueryDevice::Device deviceA;
        deviceA.name() = devName;
        deviceA.setNoChannels(8);

        AOChannelMap *mapA = deviceA.channelMap();
        deviceA.loadChannelMap();
        testAudioChannelMappingFor8Channels(mapA, expectDefault);

        setAudioChannelFor8Channels(mapA, expectLoaded);
        testAudioChannelMappingFor8Channels(mapA, expectLoaded);
        EXPECT_TRUE(mapA->setStereoType(AOChannelMap::e_Surround));
        EXPECT_TRUE(mapA->setStereoLFE(false));
        EXPECT_TRUE(mapA->setStereoCenter(false));

        deviceA.saveChannelMap();
    }

    {
        AOQueryDevice::Device deviceB;
        deviceB.name() = devName;
        deviceB.setNoChannels(8);

        AOChannelMap *mapB = deviceB.channelMap();
        deviceB.loadChannelMap();
        testAudioChannelMappingFor8Channels(mapB, expectLoaded);
        EXPECT_EQ(mapB->stereoType(), AOChannelMap::e_Surround);
        EXPECT_EQ(mapB->isStereoLFE(), false);
        EXPECT_EQ(mapB->isStereoCenter(), false);
    }

    removeOldAudioChannelSettings(devName);
}

//-------------------------------------------------------------------------------------------

TEST(AOChannelMap, saveAndLoadSettingsFor8ChsUsing2ChsThen8Chs)
{
    int expectDefault[8] = { 0,  1,  2,  3,  6,  7,  4,  5 };
    int expectLoaded[8] = { 1, 0, 2, 3, 7, 6, 4, 5 };

    QString devName = "test";
    removeOldAudioChannelSettings(devName);

    {
        AOQueryDevice::Device deviceA;
        deviceA.name() = devName;
        deviceA.setNoChannels(8);

        AOChannelMap *mapA = deviceA.channelMap();
        deviceA.loadChannelMap();
        testAudioChannelMappingFor8Channels(mapA, expectDefault);

        setAudioChannelFor8Channels(mapA, expectLoaded);
        testAudioChannelMappingFor8Channels(mapA, expectLoaded);
        EXPECT_TRUE(mapA->setStereoType(AOChannelMap::e_SurroundRear));
        EXPECT_TRUE(mapA->setStereoLFE(true));
        EXPECT_TRUE(mapA->setStereoCenter(false));

        deviceA.saveChannelMap();

        mapA->setNoMappedChannels(2);
        testAudioChannelMappingFor2Channels(mapA, 0, 1);
        EXPECT_EQ(mapA->stereoType(), AOChannelMap::e_Front);
        EXPECT_EQ(mapA->isStereoLFE(), false);
        EXPECT_EQ(mapA->isStereoCenter(), false);

        EXPECT_TRUE(mapA->setChannel(e_FrontLeft, 1));
        deviceA.saveChannelMap();
    }

    {
        AOQueryDevice::Device deviceB;
        deviceB.name() = devName;
        deviceB.setNoChannels(8);

        AOChannelMap *mapB = deviceB.channelMap();
        deviceB.loadChannelMap();
        EXPECT_EQ(mapB->noDeviceChannels(), 8);
        EXPECT_EQ(mapB->noMappedChannels(), 2);
        testAudioChannelMappingFor2Channels(mapB, 1, 0);
        EXPECT_EQ(mapB->stereoType(), AOChannelMap::e_Front);
        EXPECT_EQ(mapB->isStereoLFE(), false);
        EXPECT_EQ(mapB->isStereoCenter(), false);

        mapB->setNoMappedChannels(8);
        deviceB.loadChannelMap();
        EXPECT_EQ(mapB->noDeviceChannels(), 8);
        EXPECT_EQ(mapB->noMappedChannels(), 8);
        testAudioChannelMappingFor8Channels(mapB, expectLoaded);

        EXPECT_EQ(mapB->stereoType(), AOChannelMap::e_SurroundRear);
        EXPECT_EQ(mapB->isStereoLFE(), true);
        EXPECT_EQ(mapB->isStereoCenter(), false);
        deviceB.saveChannelMap();
    }

    {
        AOQueryDevice::Device deviceC;
        deviceC.name() = devName;
        deviceC.setNoChannels(8);

        AOChannelMap *mapC = deviceC.channelMap();
        deviceC.loadChannelMap();
        EXPECT_EQ(mapC->noDeviceChannels(), 8);
        EXPECT_EQ(mapC->noMappedChannels(), 8);
        testAudioChannelMappingFor8Channels(mapC, expectLoaded);

        EXPECT_EQ(mapC->stereoType(), AOChannelMap::e_SurroundRear);
        EXPECT_EQ(mapC->isStereoLFE(), true);
        EXPECT_EQ(mapC->isStereoCenter(), false);
    }

    removeOldAudioChannelSettings(devName);
}

//-------------------------------------------------------------------------------------------
