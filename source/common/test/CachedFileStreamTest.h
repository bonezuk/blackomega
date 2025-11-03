//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_CACHEDFILESTREAMTEST_H
#define __OMEGA_COMMON_CACHEDFILESTREAMTEST_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <QCoreApplication>
#include <QtTest/QtTest>

#include "common/inc/CachedFileStream.h"
#include "common/test/BIOStreamFunctionalTest.h"

using namespace omega;
using namespace common;
using namespace testing;

//-------------------------------------------------------------------------------------------

class CachedFileStreamQtUnitTest : public QObject
{
    public:
        Q_OBJECT
    private slots:
        void readFromEmptyFile();
        void readOnFileBoundaries();
    
        void readFileUncachedFromBeginningToEnd();
        void readFileAfterCachingFromBeginningToEnd();
        void readFileFromBeginningToEndOver4LoopsWithLookAheadCaching();
    
        void readFileFromRandomLocationsUncached();
        void readFileFromRandomLocationsWithCachingLarge();
        void readFileFromRandomLocationsWithCachingSmall();

        void cachingAllFileAndSetRange();
};

//-------------------------------------------------------------------------------------------

class CachedFileStreamQtUnitApplication : public QCoreApplication
{
    public:
        Q_OBJECT
    public:
        CachedFileStreamQtUnitApplication(int argc,char **argv);
        virtual ~CachedFileStreamQtUnitApplication();
        bool testSucceeded() const;
        
    private:
        bool m_succeeded;
    private slots:
        void runTests();
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
