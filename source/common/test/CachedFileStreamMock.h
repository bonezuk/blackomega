//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_CACHEDFILESTREAMMOCK_H
#define __OMEGA_COMMON_CACHEDFILESTREAMMOCK_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/CachedFileStream.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class CachedFileStreamMock : public CachedFileStream
{
    public:
        CachedFileStreamMock();
        virtual ~CachedFileStreamMock();

        MOCK_METHOD1(open,bool(const QString& name));
        MOCK_METHOD0(close,void());

        MOCK_METHOD3(read,tint(tchar *mem,tint64 fromPosition,tint noBytes));

        MOCK_CONST_METHOD2(isCached,bool(tint64 fromPosition,tint noBytes));
        MOCK_METHOD2(cache,tint(tint64 fromPosition,tint noBytes));

        MOCK_CONST_METHOD0(size,tint64());

        MOCK_METHOD2(drop,void(tint64 fromPosition,tint noBytes));
        MOCK_METHOD0(clear,void());

        MOCK_METHOD2(clearWithRetention,void(tint64 fromPosition,tint noBytes));

        MOCK_CONST_METHOD0(file,const AsynchronousFileReader *());
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

