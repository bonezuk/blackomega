//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_TEST_BIOTIMECACHEDSTREAMTEST_H
#define __OMEGA_COMMON_TEST_BIOTIMECACHEDSTREAMTEST_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"

#include "network/inc/Resource.h"
#include "common/inc/Random.h"
#include "common/inc/BIOTimeCachedStream.h"
#include "common/inc/BIOBufferedStream.h"
#include "engine/inc/FormatTypeFromFloat.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "common/inc/DiskOps.h"
#include "common/test/AsynchronousFileReaderMock.h"
#include "common/test/CachedFileStreamMock.h"

#include <QCoreApplication>
#include <QtTest/QtTest>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class BIOTimeCachedStreamQtUnitTest : public QObject
{
    public:
        Q_OBJECT
    private:    
        bool BIOTimeCachedStreamTesterCreateTestFile(const QString& name,tint count);
        tint *BIOTimeCachedStreamTesterCreateVBRBlockSizes(tint noBlocks,tint lowSize,tint highSize,tint seed);
        tint BIOTimeCachedStreamTesterCreateTestMultiplexedFile(const QString& name,tint noBlocks,tint lowSize,tint highSize,tint seed);
        void BIOTimeCachedStreamTesterReadAndSeekFromPositions(tint seed,BIOStreamPosition fromPosition);
                
    private slots:
    
        void openAndReadFromBeginningToEndWithConstantBitRate();
        void openAndReadBeginningToEndWithVariableBitRate();
        void openAndReadMultiplexedVBRStreamBeginningToEnd();
        
        void readAndSeekFromBeginningPositions();
        void readAndSeekFromCurrentPositions();
        void readAndSeekFromEndPositions();
        
        void readBlocksWithSequentialSeeksInBlockFromMultiplePositions();

};

//-------------------------------------------------------------------------------------------

class BIOTimeCachedStreamQtUnitApplication : public QCoreApplication
{
    public:
        Q_OBJECT
    public:
        BIOTimeCachedStreamQtUnitApplication(int argc,char **argv);
        virtual ~BIOTimeCachedStreamQtUnitApplication();
        bool testSucceeded() const;
        
    private:
        bool m_succeeded;
    private slots:
        void runTests();
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
