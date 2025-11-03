//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_WAVINFOTEST_H
#define __OMEGA_TRACK_INFO_WAVINFOTEST_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "network/inc/Resource.h"
#include "common/inc/DiskOps.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "track/info/inc/WavInfo.h"

#include <QCoreApplication>
#include <QtTest/QtTest>

using namespace omega;
using namespace track::info;
using namespace testing;

//-------------------------------------------------------------------------------------------

class WavInfoQtUnitTest : public QObject
{
    public:
        Q_OBJECT

    private Q_SLOTS:
        void readAndTestSample();
};

//-------------------------------------------------------------------------------------------

class WavInfoQtUnitApplication : public QCoreApplication
{
    public:
        Q_OBJECT

    public:
        WavInfoQtUnitApplication(int argc,char **argv);
        virtual ~WavInfoQtUnitApplication();
        bool testSucceeded() const;

    private:
        bool m_succeeded;

    private Q_SLOTS:
        void runTests();
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
