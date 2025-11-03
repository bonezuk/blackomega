//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_TEST_BIOSTREAMFUNCTIONTEST_H
#define __OMEGA_COMMON_TEST_BIOSTREAMFUNCTIONTEST_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/BIOStream.h"
#include "common/inc/BIOBufferedStream.h"
#include "common/inc/BIOMemoryStream.h"
#include "common/inc/BIOMemory.h"
#include "common/inc/DiskOps.h"
#include "track/model/test/TrackDBTestEnviroment.h"

//-------------------------------------------------------------------------------------------

using namespace omega;
using namespace common;
using namespace testing;

//-------------------------------------------------------------------------------------------

class BIOStreamFunctionalTest
{
    public:
        BIOStreamFunctionalTest();
        virtual ~BIOStreamFunctionalTest();
        
        virtual bool writeUShortTestFile(BIOStream *pFile,tint64 noElements,tint elementsPerWrite);
        
        virtual bool verifyUShortTestFileSize(BIOStream *pFile,tint64 noElements);
        virtual bool verifyUShortTestFileContents(BIOStream *pFile,tint elementsPerRead);

        virtual bool verifyFileContentsFromBothDirections(BIOStream *pFile,tint elementsPerRead);
        
        virtual bool verifyIsExpectedContents(QSharedPointer<QByteArray> pData,tint fromPosition,tint length);
        virtual bool verifyIsExpectedContents(const tchar *mem,tint fromPosition,tint length);
        
    protected:
    
        virtual tushort expectedValueAt(tint64 position,bool isGoogleTest) const;
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
