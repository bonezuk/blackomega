//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_TEST_ADATAMOCK_H
#define __OMEGA_ENGINE_TEST_ADATAMOCK_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/AData.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace omega::engine;
using namespace omega;
using namespace testing;

//-------------------------------------------------------------------------------------------

class ADataMock : public AData
{
    public:
        ADataMock();
        virtual ~ADataMock();
        
        MOCK_CONST_METHOD0(noInChannels,tint());
        MOCK_CONST_METHOD0(noOutChannels,tint());

        MOCK_METHOD0(data,sample_t *());
        MOCK_CONST_METHOD0(dataConst,const sample_t *());
        MOCK_METHOD1(data,sample_t *(tint no));
        MOCK_CONST_METHOD1(dataConst,const sample_t *(tint no));

        MOCK_METHOD0(dataOut,sample_t *());
        MOCK_CONST_METHOD0(dataOutConst,const sample_t *());
        MOCK_METHOD1(dataOut,sample_t *(tint no));
        MOCK_CONST_METHOD1(dataOutConst,const sample_t *(tint no));

        MOCK_CONST_METHOD0(frames,tint());
        MOCK_CONST_METHOD0(length,tint());

        MOCK_CONST_METHOD0(isDone,bool());
        MOCK_CONST_METHOD0(done,tint());
        MOCK_METHOD0(reset,void());
        MOCK_METHOD0(increment,void());

        MOCK_METHOD0(start,common::TimeStamp&());
        MOCK_CONST_METHOD0(startConst,const common::TimeStamp& ());

        MOCK_METHOD0(end,common::TimeStamp&());
        MOCK_CONST_METHOD0(endConst,const common::TimeStamp&());

        MOCK_CONST_METHOD0(isComplete,bool());
        MOCK_METHOD1(setComplete,void(bool flag));

        MOCK_METHOD0(mixChannels,void());
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

