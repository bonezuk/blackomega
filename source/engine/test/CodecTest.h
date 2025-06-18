//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_TEST_CODECTEST_H
#define __OMEGA_COMMON_TEST_CODECTEST_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <QCoreApplication>
#include <QtTest/QtTest>

#include "engine/inc/Codec.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "common/inc/DiskOps.h"

using namespace omega;
using namespace omega::engine;
using namespace testing;

//-------------------------------------------------------------------------------------------

class CodecQtUnitTest : public QObject
{
    public:
        Q_OBJECT
    private slots:
        void bitsPerSecondAIFF();
        void bitsPerSecondFlacCompression0();
        void bitsPerSecondFlacCompression5();
        void bitsPerSecondFlacCompression8();
        void bitsPerSecondFlacCompression8NoSeek();
        void bitsPerSecondAAC96Kbps();
        void bitsPerSecondAAC128Kbps();
        void bitsPerSecondAAC256Kbps();
        void bitsPerSecondALAC();
        void bitsPerSecondMP3At96Kbps();
        void bitsPerSecondMP3At128Kbps();
        void bitsPerSecondMP3At256Kbps();
        void bitsPerSecondMP3AtVBR();
        void bitsPerSecondMusePackAt129kbps();
        void bitsPerSecondMusePackAt291kbps();
        void bitsPerSecondVorisOggAt96kbps();
        void bitsPerSecondVorisOggAt128kbps();
        void bitsPerSecondVorisOggAt256kbps();
        void bitsPerSecondWAV();
        void bitsPerSecondWavPackLosslessAt96kbps();
        void bitsPerSecondWavPackLosslessAt256kbps();
        void bitsPerSecondWavPackLossyAt96kbps();
        void bitsPerSecondWavPackLossyAt256kbps();
        void bitsPerSecondALACCaf();
        void testNoIntegerSupportInMp3();
        void wavNativeIntegerSupport();
};

//-------------------------------------------------------------------------------------------

class CodecQtUnitApplication : public QCoreApplication
{
    public:
        Q_OBJECT
    public:
        CodecQtUnitApplication(int argc,char **argv);
        virtual ~CodecQtUnitApplication();
        bool testSucceeded() const;

    private:
        bool m_succeeded;
    private slots:
        void runTests();
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

