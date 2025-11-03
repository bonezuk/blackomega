//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_TEST_ASIODATAMOCK_H
#define __OMEGA_AUDIOIO_TEST_ASIODATAMOCK_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/ASIOData.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class ASIODataMock : public ASIOData
{
    public:
        ASIODataMock();
        virtual ~ASIODataMock();

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
        
        MOCK_METHOD0(nextPart,engine::RData::Part&());

        MOCK_CONST_METHOD0(rOffset,tint());
        MOCK_CONST_METHOD0(rLength,tint());

        MOCK_CONST_METHOD0(noParts,tint());

        MOCK_METHOD1(part,engine::RData::Part&(tint i));
        MOCK_CONST_METHOD1(partConst,const engine::RData::Part&(tint i));

        MOCK_METHOD1(partData,sample_t *(tint i));
        MOCK_CONST_METHOD1(partDataConst,const sample_t *(tint i));

        MOCK_METHOD1(partDataOut,sample_t *(tint i));
        MOCK_CONST_METHOD1(partDataOutConst,const sample_t *(tint i));

        MOCK_METHOD0(currentPartData,sample_t *());
        MOCK_CONST_METHOD0(currentPartDataConst,const sample_t *());

        MOCK_METHOD1(clipToTime,void(const common::TimeStamp& clipT));
        
        MOCK_METHOD2(asioData,void *(tint chIdx,tint pIdx));
        MOCK_CONST_METHOD2(asioDataConst,const void *(tint chIdx,tint pIdx));
        
        MOCK_METHOD1(setSampleType,void(ASIOSampleType t));
        MOCK_CONST_METHOD0(getSampleSize,tint());
        
        MOCK_METHOD1(setVolume,void(sample_t v));
        MOCK_METHOD0(convert,void());
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
