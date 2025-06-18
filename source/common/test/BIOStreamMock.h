//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_TEST_BIOSTREAMMOCK_H
#define __OMEGA_COMMON_TEST_BIOSTREAMMOCK_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BIOStream.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
namespace test
{
//-------------------------------------------------------------------------------------------

class BIOStreamMock : public BIOStream
{
    public:
        BIOStreamMock(int flags);

        MOCK_CONST_METHOD0(name,const QString&());

        MOCK_METHOD1(open,bool(const tchar *name));
        MOCK_METHOD1(open,bool(const BString& name));
        MOCK_METHOD1(open,bool(const QString& name));

        MOCK_METHOD0(close,bool());

        MOCK_METHOD2(read,tint(tbyte *mem,tint len));
        MOCK_METHOD2(read,tint(tubyte *mem,tint len));

        MOCK_METHOD2(write,tint(const tbyte *mem,tint len));
        MOCK_METHOD2(write,tint(const tubyte *mem,tint len));

        MOCK_METHOD2(seek,bool(tint pos,BIOStreamPosition flag));

        MOCK_METHOD0(sof,bool());
        MOCK_METHOD0(eof,bool());

        MOCK_METHOD0(size,tint());

        MOCK_METHOD1(bookmark,tint(tint offset));

        MOCK_METHOD1(deleteBookmark,void(tint bk));

        MOCK_METHOD1(position,bool(tint bk));

        MOCK_CONST_METHOD0(readable,bool());
        MOCK_CONST_METHOD0(writeable,bool());

        MOCK_CONST_METHOD0(offset,tint());
};

//-------------------------------------------------------------------------------------------
} // namespace test
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
