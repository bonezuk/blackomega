//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_TEST_ASYNCHRONOUSFILEREADERMOCK_H
#define __OMEGA_COMMON_TEST_ASYNCHRONOUSFILEREADERMOCK_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/AsynchronousFileReader.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class AsynchronousFileReaderMock : public AsynchronousFileReader
{
    public:
        AsynchronousFileReaderMock();
        virtual ~AsynchronousFileReaderMock();

        MOCK_CONST_METHOD0(name,const QString&());
        MOCK_METHOD1(open,bool(const QString& name));
        MOCK_METHOD0(close,void());
        MOCK_METHOD2(read,tint(tint fromPosition,tint len));
        MOCK_METHOD2(read,tint(tint64 fromPosition,tint len));
        MOCK_CONST_METHOD0(size,tint());
        MOCK_CONST_METHOD0(size64,tint64());
        MOCK_METHOD0(hasCompleted,QList<int>());
        MOCK_METHOD1(result,QSharedPointer<QByteArray>(tint readId));
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
