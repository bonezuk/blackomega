//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_TEST_AUDIOITEMMOCK_H
#define __OMEGA_AUDIOIO_TEST_AUDIOITEMMOCK_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/AOBase.h"

using namespace omega::audioio;
using namespace omega;
using namespace testing;

//-------------------------------------------------------------------------------------------

class AudioItemMock : public AudioItem
{
    public:
        AudioItemMock();
        virtual ~AudioItemMock();
    
        MOCK_METHOD0(prev,AudioItem *());
        MOCK_CONST_METHOD0(prevConst,const AudioItem *());
        MOCK_METHOD1(setPrev,void(AudioItem *item));
        MOCK_METHOD0(next,AudioItem *());
        MOCK_CONST_METHOD0(nextConst,const AudioItem *());
        MOCK_METHOD1(setNext,void(AudioItem *item));
        MOCK_CONST_METHOD0(state,AudioItem::ItemStates());
        MOCK_METHOD1(setState,void(AudioItem::ItemStates v));
        MOCK_CONST_METHOD0(done,tint());
        MOCK_METHOD1(setDone,void(tint v));
        MOCK_METHOD0(data,engine::AData *());
        MOCK_CONST_METHOD0(dataConst,const engine::AData *());
        MOCK_METHOD1(setData,void(engine::AData *d));
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
