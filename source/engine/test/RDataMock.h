//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_TEST_RDATAMOCK_H
#define __OMEGA_ENGINE_TEST_RDATAMOCK_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/RData.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace omega::engine;
using namespace omega;
using namespace testing;

//-------------------------------------------------------------------------------------------

class RDataMock : public RData
{
	public:
		RDataMock();
		virtual ~RDataMock();

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
		
		MOCK_METHOD0(nextPart,RData::Part&());

		MOCK_CONST_METHOD0(rOffset,tint());
		MOCK_CONST_METHOD0(rLength,tint());

		MOCK_CONST_METHOD0(noParts,tint());

		MOCK_METHOD1(part,RData::Part&(tint i));
		MOCK_CONST_METHOD1(partConst,const RData::Part&(tint i));

		MOCK_METHOD1(partData,sample_t *(tint i));
        MOCK_CONST_METHOD1(partDataConst,const sample_t *(tint i));

		MOCK_METHOD1(partDataOut,sample_t *(tint i));
        MOCK_CONST_METHOD1(partDataOutConst,const sample_t *(tint i));

		MOCK_METHOD0(currentPartData,sample_t *());
        MOCK_CONST_METHOD0(currentPartDataConst,const sample_t *());

		MOCK_METHOD1(clipToTime,void(const common::TimeStamp& clipT));
		
		MOCK_CONST_METHOD1(partGetDataType, CodecDataType(tint i));
};

//-------------------------------------------------------------------------------------------

class RDataPartMock : public RData::Part
{
	public:
		RDataPartMock();
		virtual ~RDataPartMock();

		MOCK_METHOD0(offset,tint&());
		MOCK_CONST_METHOD0(offsetConst,const tint&());
		MOCK_METHOD0(length,tint&());
		MOCK_CONST_METHOD0(lengthConst,const tint&());
		MOCK_METHOD0(done,bool&());
		MOCK_CONST_METHOD0(doneConst,const bool&());

		MOCK_METHOD0(start,common::TimeStamp&());
		MOCK_CONST_METHOD0(startConst,const common::TimeStamp&());
		MOCK_METHOD0(end,common::TimeStamp&());
		MOCK_CONST_METHOD0(endConst,const common::TimeStamp&());

		MOCK_METHOD0(startNext,common::TimeStamp&());
		MOCK_CONST_METHOD0(startNextConst,const common::TimeStamp&());
		MOCK_METHOD0(endNext,common::TimeStamp&());
		MOCK_CONST_METHOD0(endNextConst,const common::TimeStamp&());

		MOCK_METHOD0(refStartTime,common::TimeStamp&());
		MOCK_CONST_METHOD0(refStartTimeConst,const common::TimeStamp&());

		MOCK_CONST_METHOD0(isNext,bool());
		MOCK_METHOD1(setNext,void(bool flag));
		
		MOCK_CONST_METHOD0(getDataType, CodecDataType());
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
