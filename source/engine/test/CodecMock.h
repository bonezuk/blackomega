//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_TEST_CODECMOCK_H
#define __OMEGA_ENGINE_TEST_CODECMOCK_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/Codec.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace omega::engine;
using namespace omega;
using namespace testing;

//-------------------------------------------------------------------------------------------

class CodecMock : public Codec
{
    public:
        CodecMock();
        virtual ~CodecMock();

        MOCK_CONST_METHOD0(type,Codec::CodecType());

        MOCK_METHOD1(open,bool(const QString& name));
        MOCK_METHOD0(close,void());
        MOCK_CONST_METHOD0(isInitialized,bool());
        MOCK_METHOD0(init,bool());
        MOCK_METHOD1(next,bool(AData& data));
        MOCK_CONST_METHOD0(isSeek,bool());
        MOCK_METHOD1(seek,bool(const common::TimeStamp& v));
        MOCK_CONST_METHOD0(isComplete,bool());
        MOCK_CONST_METHOD0(isRemote,bool());
        MOCK_METHOD1(isBuffered,bool(tfloat32& percent));
        MOCK_METHOD1(buffer,void(const common::TimeStamp& bLen));
        MOCK_METHOD0(resync,void());
        MOCK_CONST_METHOD0(bitrate,tint());
        MOCK_CONST_METHOD0(frequency,tint());
        MOCK_CONST_METHOD0(noChannels,tint());
        MOCK_CONST_METHOD0(length,common::TimeStamp());
        MOCK_CONST_METHOD0(loadFileToMemory,bool());
        MOCK_CONST_METHOD0(noOutputChannels,tint());
        MOCK_METHOD1(setNoOutputChannels,void(tint noChs));

        MOCK_CONST_METHOD0(dataTypesSupported, CodecDataType());
        MOCK_METHOD1(setDataTypeFormat, bool(CodecDataType type));
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

