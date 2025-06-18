#include "network/inc/Resource.h"

#include "gtest/gtest.h"

using namespace omega;
using namespace network;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(Resource,networkIPAddressToStringExternal)
{
    // 74.125.24.105 (www.google.com)
    tuint32 ipNative = 0x4a7d1869;

    struct in_addr ipAddress;
    ipAddress.s_addr = ntohl(ipNative);
    
    QString ipText = Resource::instance().networkIPAddressToString(ipAddress);
    
    EXPECT_TRUE(ipText=="74.125.24.105");
}

//-------------------------------------------------------------------------------------------

TEST(Resource,networkIPAddressToStringPrivate)
{
    // 192.168.5.9
    tuint32 ipNative = 0xc0a80509;

    struct in_addr ipAddress;
    ipAddress.s_addr = ntohl(ipNative);
    
    QString ipText = Resource::instance().networkIPAddressToString(ipAddress);
    
    EXPECT_TRUE(ipText=="192.168.5.9");
}

//-------------------------------------------------------------------------------------------
