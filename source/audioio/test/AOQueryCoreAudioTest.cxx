#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/AOQueryCoreAudio.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(AOQueryCoreAudio,deviceHasExpectedType)
{
    AOQueryCoreAudio::DeviceCoreAudio deviceA;
    ASSERT_EQ(AOQueryDevice::Device::e_deviceCoreAudio,deviceA.type());

    AOQueryCoreAudio::DeviceCoreAudio deviceB(deviceA);
    ASSERT_EQ(AOQueryDevice::Device::e_deviceCoreAudio,deviceB.type());
}

//-------------------------------------------------------------------------------------------
