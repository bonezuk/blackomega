//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_TEST_REMOTEIFMOCK_H
#define __OMEGA_REMOTE_TEST_REMOTEIFMOCK_H
//-------------------------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "remote/inc/RemoteIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class RemoteIFMock : public RemoteIF
{
    public:
        MOCK_METHOD0(start,bool());
        MOCK_METHOD0(stop,void());
        MOCK_METHOD1(remoteEvent,void(RemoteEvent *e));
        MOCK_METHOD0(onTimerImplementation,void());
};

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
