#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/AOQueryASIO.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(AOQueryASIO,deviceHasExpectedType)
{
    AOQueryDevice::Device deviceA(AOQueryDevice::Device::e_deviceASIO);
    ASSERT_EQ(AOQueryDevice::Device::e_deviceASIO,deviceA.type());

    AOQueryDevice::Device deviceB(deviceA);
    ASSERT_EQ(AOQueryDevice::Device::e_deviceASIO,deviceB.type());
}

//-------------------------------------------------------------------------------------------
