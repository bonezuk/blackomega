//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_TEST_AOBASETEST_H
#define __OMEGA_AUDIOIO_TEST_AOBASETEST_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"
#include "audioio/inc/AOBase.h"
#include "engine/test/CodecMock.h"
#include "audioio/test/AudioItemMock.h"
#include "engine/test/ADataMock.h"
#include "engine/test/RDataMock.h"
#include "audioio/test/AudioHardwareBufferTester.h"
#include "audioio/inc/SampleConverter.h"

#include <QtTest/QtTest>

using namespace omega::audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

class AOBaseQtUnitTest : public QObject
{
	public:
		Q_OBJECT
	private slots:
		void setExclusiveSettingsAreSpecificToActiveDevice();
		void setExclusiveInNoCodecState();
		void setExclusiveInPreBufferState();
		void setExclusiveInPlayState();
		void setExclusiveInCrossFadeState();
		void setExclusiveInPauseState();
		void setExclusiveInStopState();
};

//-------------------------------------------------------------------------------------------

class AOBaseQtUnitApplication : public QCoreApplication
{
	public:
		Q_OBJECT
	public:
		AOBaseQtUnitApplication(int argc,char **argv);
		virtual ~AOBaseQtUnitApplication();
		bool testSucceeded() const;
		
	private:
		bool m_succeeded;
	private slots:
		void runTests();
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------