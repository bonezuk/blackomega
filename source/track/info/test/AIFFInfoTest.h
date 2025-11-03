//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_AIFFINFOTEST_H
#define __OMEGA_TRACK_INFO_AIFFINFOTEST_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "network/inc/Resource.h"
#include "common/inc/DiskOps.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "track/info/inc/AIFFInfo.h"

#include <QCoreApplication>
#include <QtTest/QtTest>

using namespace omega;
using namespace track::info;
using namespace testing;

//-------------------------------------------------------------------------------------------

class AIFFInfoQtUnitTest : public QObject
{
    public:
        Q_OBJECT

    private Q_SLOTS:
        void readInfo();
};

//-------------------------------------------------------------------------------------------

class AIFFInfoQtUnitApplication : public QCoreApplication
{
    public:
        Q_OBJECT

    public:
        AIFFInfoQtUnitApplication(int argc,char **argv);
        virtual ~AIFFInfoQtUnitApplication();
        bool testSucceeded() const;

    private:
        bool m_succeeded;

    private Q_SLOTS:
        void runTests();
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
