#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/AOQueryWin32.h"
#include "audioio/inc/WasAPIIF.h"
#include "audioio/inc/WasAPICOMInterfaces.h"
#include "audioio/inc/WasAPILayerIF.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(AOQueryWin32,queryDevices)
{
    ASSERT_TRUE(ASIODriverService::start());

    WasAPIIF::setExclusive(true);
    WasAPIIFSPtr pWAS = WasAPIIF::instance("wasapi");
    ASSERT_FALSE(pWAS.isNull());

    ASIODriverService& asioService = ASIODriverService::instance();

    AOQueryDevice *deviceInfo = new AOQueryWin32;

    ASSERT_TRUE(deviceInfo->queryNames());

    for(int i=0;i<deviceInfo->noDevices();i++)
    {
        ASSERT_TRUE(deviceInfo->queryDevice(i));
    }

    deviceInfo->print();

    delete deviceInfo;

    ASIODriverService::stop();
    WasAPIIF::release();
}

//-------------------------------------------------------------------------------------------
