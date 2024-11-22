#include "engine/test/CodecTest.h"

#include <QApplication>

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondAIFF()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "aiff/bps_16bit_2ch.aiff");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	// 16 * 2 * 44100
	QCOMPARE(16 * 2 * 44100,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondFlacCompression0()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "flac/bps_16bit_2ch_comp0.flac");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(692570,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondFlacCompression5()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "flac/bps_16bit_2ch_comp5.flac");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(657173,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondFlacCompression8()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "flac/bps_16bit_2ch_comp8.flac");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(655024,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondFlacCompression8NoSeek()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "flac/bps_16bit_2ch_comp8_noseek.flac");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(655024,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondAAC96Kbps()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "m4a_aac/bps_16bit_2ch_96kbps.m4a");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(92736,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondAAC128Kbps()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "m4a_aac/bps_16bit_2ch_128kbps.m4a");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(123440,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondAAC256Kbps()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "m4a_aac/bps_16bit_2ch_256kbps.m4a");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(256664,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondALAC()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "m4a_alac/bps_16bit_2ch.m4a");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
    QCOMPARE(641076,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondMP3At96Kbps()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "mp3/bps_16bit_2ch_96kbps.mp3");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(96000,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(32000,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::testNoIntegerSupportInMp3()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "mp3/bps_16bit_2ch_96kbps.mp3");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(engine::e_SampleFloat, codec->dataTypesSupported());
	QCOMPARE(true, codec->setDataTypeFormat(engine::e_SampleFloat));
	QCOMPARE(false, codec->setDataTypeFormat(engine::e_SampleInt16));
	QCOMPARE(false, codec->setDataTypeFormat(engine::e_SampleInt16 | engine::e_SampleFloat));
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondMP3At128Kbps()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "mp3/bps_16bit_2ch_128kbps.mp3");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(128000,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondMP3At256Kbps()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "mp3/bps_16bit_2ch_256kbps.mp3");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(256000,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondMP3AtVBR()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "mp3/bps_16bit_2ch_vbr.mp3");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(87753,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(24000,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondMusePackAt129kbps()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "mpc/bps_16bit_2ch_q3_129.2kbps.mpc");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(129205,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondMusePackAt291kbps()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "mpc/bps_16bit_2ch_q7_291.8kbps.mpc");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(291797,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondVorisOggAt96kbps()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "ogg/bps_16bit_2ch_96kpbs.ogg");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(90682,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondVorisOggAt128kbps()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "ogg/bps_16bit_2ch_128kpbs.ogg");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(114917,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondVorisOggAt256kbps()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "ogg/bps_16bit_2ch_256kpbs.ogg");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(241520,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondWAV()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "wav/bps_16bit_2ch.wav");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	// 16 * 2 * 44100
	QCOMPARE(16 * 2 * 44100,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::wavNativeIntegerSupport()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "wav/bps_16bit_2ch.wav");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	QCOMPARE(engine::e_SampleFloat | engine::e_SampleInt16, codec->dataTypesSupported());
	QCOMPARE(true, codec->setDataTypeFormat(engine::e_SampleInt16));
	QCOMPARE(false, codec->setDataTypeFormat(engine::e_SampleInt24));
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondWavPackLosslessAt96kbps()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "wv_full/bps_16bit_2ch_96kbps.wv");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(691136,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondWavPackLosslessAt256kbps()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "wv_full/bps_16bit_2ch_256kbps.wv");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(671434,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondWavPackLossyAt96kbps()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "wv_lossy/bps_16bit_2ch_96kbps.wv");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(199221,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondWavPackLossyAt256kbps()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "wv_lossy/bps_16bit_2ch_256kbps.wv");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(265109,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitTest::bitsPerSecondALACCaf()
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "caf/bps_16bit_2ch.caf");
	
	engine::Codec *codec = engine::Codec::get(fileName);
	QVERIFY(codec!=0);
	QVERIFY(codec->init());
	
	QCOMPARE(4948,codec->bitrate());
	QCOMPARE(2,codec->noChannels());
	QCOMPARE(44100,codec->frequency());
	
	delete codec;
}

//-------------------------------------------------------------------------------------------

CodecQtUnitApplication::CodecQtUnitApplication(int argc,char **argv) : QCoreApplication(argc,argv),
    m_succeeded(false)
{
	QTimer::singleShot(10,this,SLOT(runTests()));
}

//-------------------------------------------------------------------------------------------

CodecQtUnitApplication::~CodecQtUnitApplication()
{}

//-------------------------------------------------------------------------------------------

bool CodecQtUnitApplication::testSucceeded() const
{
	return m_succeeded;
}

//-------------------------------------------------------------------------------------------

void CodecQtUnitApplication::runTests()
{
	CodecQtUnitTest tests;
	m_succeeded = (QTest::qExec(&tests)==0) ? true : false;
	quit();
}

//-------------------------------------------------------------------------------------------

TEST(CodecQtUnitApplication,RunQtUnitTests)
{
    QStringList libPaths = QApplication::libraryPaths();
    {
        char *argv = 0;
        CodecQtUnitApplication tester(0,&argv);
        tester.exec();
        ASSERT_TRUE(tester.testSucceeded());
    }
    QApplication::setLibraryPaths(libPaths);
}

//-------------------------------------------------------------------------------------------
