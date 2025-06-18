//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_TEST_ASIODRIVERMOCK_H
#define __OMEGA_AUDIOIO_TEST_ASIODRIVERMOCK_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/ASIODriver.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace omega::audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

class ASIODriverMock : public ASIODriver
{
    public:
        ASIODriverMock();
        ~ASIODriverMock();

        MOCK_METHOD1(ASIOInit,ASIOError(ASIODriverInfo *info));
        MOCK_METHOD0(ASIOExit,ASIOError());

        MOCK_METHOD0(ASIOStart,ASIOError());
        MOCK_METHOD0(ASIOStop,ASIOError());

        MOCK_METHOD2(ASIOGetChannels,ASIOError(long *numInputChannels, long *numOutputChannels));
        MOCK_METHOD2(ASIOGetLatencies,ASIOError(long *inputLatency, long *outputLatency));
        MOCK_METHOD4(ASIOGetBufferSize,ASIOError(long *minSize, long *maxSize, long *preferredSize, long *granularity));
        MOCK_METHOD1(ASIOCanSampleRate,ASIOError(ASIOSampleRate sampleRate));
        MOCK_METHOD1(ASIOGetSampleRate,ASIOError(ASIOSampleRate *currentRate));
        MOCK_METHOD1(ASIOSetSampleRate,ASIOError(ASIOSampleRate sampleRate));
        MOCK_METHOD2(ASIOGetClockSources,ASIOError(ASIOClockSource *clocks, long *numSources));
        MOCK_METHOD1(ASIOSetClockSource,ASIOError(long reference));
        MOCK_METHOD2(ASIOGetSamplePosition,ASIOError(ASIOSamples *sPos, ASIOTimeStamp *tStamp));
        MOCK_METHOD1(ASIOGetChannelInfo,ASIOError(ASIOChannelInfo *info));

        MOCK_METHOD4(ASIOCreateBuffers,ASIOError(ASIOBufferInfo *bufferInfos, long numChannels,long bufferSize, ASIOCallbacks *callbacks));

        MOCK_METHOD0(ASIODisposeBuffers,ASIOError());
        MOCK_METHOD0(ASIOControlPanel,ASIOError());

        MOCK_METHOD2(ASIOFuture,ASIOError(long selector, void *params));

        MOCK_METHOD0(ASIOOutputReady,ASIOError());

        MOCK_METHOD0(service,ASIODriverService *());
        MOCK_METHOD0(driver,IASIO *());
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------


