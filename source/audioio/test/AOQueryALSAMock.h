//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_TEST_AOQUERYALSAMOCK_H
#define __OMEGA_AUDIOIO_TEST_AOQUERYALSAMOCK_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/AOQueryALSA.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

class AOQueryALSADeviceMock : public AOQueryALSA::DeviceALSA
{
    public:
        MOCK_CONST_METHOD0(isInitialized,bool());
        MOCK_METHOD0(setInitialized,void());
        
        MOCK_METHOD0(id,QString&());
        MOCK_CONST_METHOD0(id,const QString&());
        
        MOCK_METHOD0(name,QString&());
        MOCK_CONST_METHOD0(name,const QString&());

        MOCK_CONST_METHOD1(isFrequencySupported,bool(int freq));
        MOCK_METHOD1(addFrequency,void(int freq));
        MOCK_CONST_METHOD0(frequencies,const QSet<int>&());

        MOCK_CONST_METHOD0(noChannels,int());
        MOCK_METHOD1(setNoChannels,void(int noCh));

        MOCK_CONST_METHOD0(print,void());
        
        MOCK_CONST_METHOD1(isSupported,bool(const FormatDescription& desc));
        MOCK_CONST_METHOD0(supported,const FormatsSupported&());
        MOCK_METHOD1(queryDevice,bool(int cardNo));
        MOCK_CONST_METHOD0(pcmDeviceName,QString());
        MOCK_CONST_METHOD1(formatsFromDescription,QVector<int>(const FormatDescription& desc));
        MOCK_CONST_METHOD4(canSupportFormat,bool(snd_pcm_t *handle,tint fType,tint noChannels,tint frequency));
};
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
