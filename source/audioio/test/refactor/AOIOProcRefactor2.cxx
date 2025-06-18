WAVEFORMATEX *WasAPIDeviceLayer::findClosestSupportedFormat(const FormatDescription& sourceDesc)
{
    WAVEFORMATEX *pFormat = findClosestWaveFormatFromDescription(sourceDesc);
    
    if(pFormat!=0)
    {
        HRESULT hr;
        
        if(isExclusive())
        {
            hr = m_pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE,pFormat,0);
            if(hr!=S_OK)
            {
                WAVEFORMATEX *pDelFormat = pFormat;
            
                if(pFormat->cbSize==0)
                {
                    WAVEFORMATEXTENSIBLE *pExFormat = toWaveExtensible(pFormat);
                    
                    hr = m_pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE,reinterpret_cast<const WAVEFORMATEX *>(pExFormat),0);
                    if(hr==S_OK)
                    {
                        pFormat = pExFormat;
                    }
                    else
                    {
                        delete pExFormat;
                        pFormat = 0;                        
                    }
                }
                else
                {
                    pFormat = 0;
                }
                delete pDelFormat;
            }
        }
        else
        {
            WAVEFORMATEX *pCloseFormat = 0;
            
            hr = m_pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,pFormat,&pCloseFormat);
            if(pCloseFormat!=0)
            {
                CoTaskMemFree(pCloseFormat);
                pCloseFormat = 0;
            }

            if(hr!=S_OK)
            {
                WAVEFORMATEX *pDelFormat = pFormat;
            
                if(pFormat->cbSize==0)
                {
                    WAVEFORMATEXTENSIBLE *pExFormat = toWaveExtensible(pFormat);
                
                    hr = m_pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,reinterpret_cast<const WAVEFORMATEX *>(pExFormat),&pCloseFormat);
                    if(pCloseFormat!=0)
                    {
                        CoTaskMemFree(pCloseFormat);
                        pCloseFormat = 0;
                    }                    
                    
                    if(hr==S_OK)
                    {
                        pFormat = pExFormat;
                    }
                    else
                    {
                        delete pExFormat;
                        pFormat = 0;                        
                    }
                }
                else
                {
                    pFormat = 0;
                }
                delete pDelFormat;
            }
        }
    }
    return pFormat;
}

class WasAPIDeviceLayerFindClosestSupportedFormatTest : public WasAPIDeviceLayerTest
{
    public:
        WasAPIDeviceLayerFindClosestSupportedFormatTest(IAudioClientIFSPtr pAudioClient);
        WAVEFORMATEXTENSIBLE *toWaveExtensible(WAVEFORMATEX *pFormat) const;
        MOCK_METHOD1(findClosestWaveFormatFromDescription,WAVEFORMATEX *(const FormatDescription& sourceDesc));
};

WasAPIDeviceLayerFindClosestSupportedFormatTest::WasAPIDeviceLayerFindClosestSupportedFormatTest(IAudioClientIFSPtr pAudioClient) : WasAPIDeviceLayerTest(IMMDeviceIFSPtr(),pAudioClient)
{}

WAVEFORMATEXTENSIBLE *WasAPIDeviceLayerFindClosestSupportedFormatTest::toWaveExtensible(WAVEFORMATEX *pFormat) const
{
    return WasAPIDeviceLayer::toWaveExtensible(pFormat);
}

MATCHER_P(WaveFormatExIsExpected,pExpectFormat,"") { return (memcmp(arg,pExpectFormat,sizeof(WAVEFORMATEX))==0); }
MATCHER_P(WaveFormatExtensibleIsExpected,pExpectFormat,"") { return (memcmp(arg,pExpectFormat,sizeof(WAVEFORMATEXTENSIBLE))==0); }

TEST(WasAPIDeviceLayer,findClosestSupportedFormatWhenClosestNotFound)
{
    WasAPIIF::setExclusive(true);
    WasAPIIFSPtr pAPI = WasAPIIF::instance("mock");
    WasAPIIFMock& apiMock = dynamic_cast<WasAPIIFMock&>(*(pAPI.data()));
    EXPECT_CALL(apiMock,isExclusive()).WillRepeatedly(Return(true));

    FormatDescription sourceDesc;

    IAudioClientIFSPtr pAudioClient(new IAudioClientIFMock());

    WasAPIDeviceLayerFindClosestSupportedFormatTest deviceLayer(pAudioClient);

    EXPECT_CALL(deviceLayer,findClosestWaveFormatFromDescription(sourceDesc)).Times(1).WillOnce(Return(0));

    WAVEFORMATEX *pActualFormat = deviceLayer.findClosestSupportedFormat(sourceDesc);
    
    ASSERT_TRUE(pActualFormat==0);
    
    WasAPIIF::release();
}

TEST(WasAPIDeviceLayer,findClosestSupportedFormatWithExclusiveAndNonExIsSupported)
{
    WasAPIIF::setExclusive(true);
    WasAPIIFSPtr pAPI = WasAPIIF::instance("mock");
    WasAPIIFMock& apiMock = dynamic_cast<WasAPIIFMock&>(*(pAPI.data()));
    EXPECT_CALL(apiMock,isExclusive()).WillRepeatedly(Return(true));

    FormatDescription sourceDesc;

    IAudioClientIFSPtr pAudioClient(new IAudioClientIFMock());
    IAudioClientIFMock& mockAudio = dynamic_cast<IAudioClientIFMock&>(*(pAudioClient.data()));

    WasAPIDeviceLayerFindClosestSupportedFormatTest deviceLayer(pAudioClient);

    WAVEFORMATEX *formatA;
    formatA = new WAVEFORMATEX;
    deviceLayer.testDefaultWaveFormat(*formatA);
    deviceLayer.testSetWaveFormat(2,16,44100,*formatA);
    
    EXPECT_CALL(deviceLayer,findClosestWaveFormatFromDescription(sourceDesc)).Times(1).WillOnce(Return(formatA));
    
    EXPECT_CALL(mockAudio,IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,WaveFormatExIsExpected(formatA),0))
        .Times(1).WillOnce(Return(S_OK));
        
    WAVEFORMATEX *pActualFormat = deviceLayer.findClosestSupportedFormat(sourceDesc);
    
    ASSERT_TRUE(pActualFormat!=0);
    EXPECT_EQ(0,memcmp(pActualFormat,formatExtA,sizeof(WAVEFORMATEX)));
    
    WasAPIIF::release();
}

TEST(WasAPIDeviceLayer,findClosestSupportedFormatWithExclusiveAndNonExIsNotSupportedAndCBSizeIsSet)
{
    WasAPIIF::setExclusive(true);
    WasAPIIFSPtr pAPI = WasAPIIF::instance("mock");
    WasAPIIFMock& apiMock = dynamic_cast<WasAPIIFMock&>(*(pAPI.data()));
    EXPECT_CALL(apiMock,isExclusive()).WillRepeatedly(Return(true));

    FormatDescription sourceDesc;

    IAudioClientIFSPtr pAudioClient(new IAudioClientIFMock());
    IAudioClientIFMock& mockAudio = dynamic_cast<IAudioClientIFMock&>(*(pAudioClient.data()));

    WasAPIDeviceLayerFindClosestSupportedFormatTest deviceLayer(pAudioClient);

    WAVEFORMATEX *formatA;
    formatA = new WAVEFORMATEX;
    deviceLayer.testDefaultWaveFormat(*formatA);
    deviceLayer.testSetWaveFormat(2,16,44100,*formatA);
    formatA->cbSize = 22;
    
    EXPECT_CALL(deviceLayer,findClosestWaveFormatFromDescription(sourceDesc)).Times(1).WillOnce(Return(formatA));
    
    EXPECT_CALL(mockAudio,IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,WaveFormatExIsExpected(formatA),0))
        .Times(1).WillOnce(Return(S_FALSE));
        
    WAVEFORMATEX *pActualFormat = deviceLayer.findClosestSupportedFormat(sourceDesc);
    
    ASSERT_TRUE(pActualFormat==0);
    
    WasAPIIF::release();
}

TEST(WasAPIDeviceLayer,findClosestSupportedFormatWithExclusiveAndNonExIsNotSupportedAndExtIsSupported)
{
    WasAPIIF::setExclusive(true);
    WasAPIIFSPtr pAPI = WasAPIIF::instance("mock");
    WasAPIIFMock& apiMock = dynamic_cast<WasAPIIFMock&>(*(pAPI.data()));
    EXPECT_CALL(apiMock,isExclusive()).WillRepeatedly(Return(true));

    FormatDescription sourceDesc;

    IAudioClientIFSPtr pAudioClient(new IAudioClientIFMock());
    IAudioClientIFMock& mockAudio = dynamic_cast<IAudioClientIFMock&>(*(pAudioClient.data()));

    WasAPIDeviceLayerFindClosestSupportedFormatTest deviceLayer(pAudioClient);

    WAVEFORMATEX *formatA;
    formatA = new WAVEFORMATEX;
    deviceLayer.testDefaultWaveFormat(*formatA);
    deviceLayer.testSetWaveFormat(2,16,44100,*formatA);
    WAVEFORMATEXTENSIBLE *formatExtA = deviceLayer.toWaveExtensible(formatA);
    
    EXPECT_CALL(deviceLayer,findClosestWaveFormatFromDescription(sourceDesc)).Times(1).WillOnce(Return(formatA));
    
    EXPECT_CALL(mockAudio,IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,WaveFormatExIsExpected(formatA),0))
        .Times(1).WillOnce(Return(S_FALSE));
    EXPECT_CALL(mockAudio,IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,WaveFormatExtensibleIsExpected(formatExtA),0))
        .Times(1).WillOnce(Return(S_OK));
    
    WAVEFORMATEX *pActualFormat = deviceLayer.findClosestSupportedFormat(sourceDesc);
    
    ASSERT_TRUE(pActualFormat!=0);
    EXPECT_EQ(0,memcmp(pActualFormat,formatExtA,sizeof(WAVEFORMATEXTENSIBLE)));
    
    delete formatExtA;
    
    WasAPIIF::release();
}

TEST(WasAPIDeviceLayer,findClosestSupportedFormatWithExclusiveAndNonExAndExtIsNotSupported)
{
    WasAPIIF::setExclusive(true);
    WasAPIIFSPtr pAPI = WasAPIIF::instance("mock");
    WasAPIIFMock& apiMock = dynamic_cast<WasAPIIFMock&>(*(pAPI.data()));
    EXPECT_CALL(apiMock,isExclusive()).WillRepeatedly(Return(true));

    FormatDescription sourceDesc;

    IAudioClientIFSPtr pAudioClient(new IAudioClientIFMock());
    IAudioClientIFMock& mockAudio = dynamic_cast<IAudioClientIFMock&>(*(pAudioClient.data()));

    WasAPIDeviceLayerFindClosestSupportedFormatTest deviceLayer(pAudioClient);

    WAVEFORMATEX *formatA;
    formatA = new WAVEFORMATEX;
    deviceLayer.testDefaultWaveFormat(*formatA);
    deviceLayer.testSetWaveFormat(2,16,44100,*formatA);
    WAVEFORMATEXTENSIBLE *formatExtA = deviceLayer.toWaveExtensible(formatA);
    
    EXPECT_CALL(deviceLayer,findClosestWaveFormatFromDescription(sourceDesc)).Times(1).WillOnce(Return(formatA));
    
    EXPECT_CALL(mockAudio,IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,WaveFormatExIsExpected(formatA),0))
        .Times(1).WillOnce(Return(S_FALSE));
    EXPECT_CALL(mockAudio,IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,WaveFormatExtensibleIsExpected(formatExtA),0))
        .Times(1).WillOnce(Return(S_FALSE));
    
    WAVEFORMATEX *pActualFormat = deviceLayer.findClosestSupportedFormat(sourceDesc);
    
    EXPECT_TRUE(pActualFormat==0);
    
    delete formatExtA;
    
    WasAPIIF::release();
}

TEST(WasAPIDeviceLayer,findClosestSupportedFormatWithSharedAndNonExIsSupported)
{
    WasAPIIF::setExclusive(false);
    WasAPIIFSPtr pAPI = WasAPIIF::instance("mock");
    WasAPIIFMock& apiMock = dynamic_cast<WasAPIIFMock&>(*(pAPI.data()));
    EXPECT_CALL(apiMock,isExclusive()).WillRepeatedly(Return(false));

    FormatDescription sourceDesc;

    IAudioClientIFSPtr pAudioClient(new IAudioClientIFMock());
    IAudioClientIFMock& mockAudio = dynamic_cast<IAudioClientIFMock&>(*(pAudioClient.data()));

    WasAPIDeviceLayerFindClosestSupportedFormatTest deviceLayer(pAudioClient);

    WAVEFORMATEX *formatA,*formatClosestA;
    formatA = new WAVEFORMATEX;
    deviceLayer.testDefaultWaveFormat(*formatA);
    deviceLayer.testSetWaveFormat(2,16,44100,*formatA);
    
    formatClosestA = CoTaskMemAlloc(sizeof(WAVEFORMATEX));
    ASSERT_TRUE(formatClosestA!=0);
    deviceLayer.testDefaultWaveFormat(*formatClosestA);
    deviceLayer.testSetWaveFormat(2,24,44100,*formatClosestA);

    EXPECT_CALL(deviceLayer,findClosestWaveFormatFromDescription(sourceDesc)).Times(1).WillOnce(Return(formatA));
    
    EXPECT_CALL(mockAudio,IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,WaveFormatExIsExpected(formatA),A<WAVEFORMATEX **>()))
        .Times(1).WillOnce(DoAll(SetArgPointee<2>(formatClosestA),Return(S_OK)));
        
    WAVEFORMATEX *pActualFormat = deviceLayer.findClosestSupportedFormat(sourceDesc);
    
    ASSERT_TRUE(pActualFormat!=0);
    EXPECT_EQ(0,memcmp(pActualFormat,formatA,sizeof(WAVEFORMATEX)));
    
    delete pActualFormat;
    
    WasAPIIF::release();
}

TEST(WasAPIDeviceLayer,findClosestSupportedFormatWithSharedAndNonExIsNotSupportedAndCBSizeIsSet)
{
    WasAPIIF::setExclusive(false);
    WasAPIIFSPtr pAPI = WasAPIIF::instance("mock");
    WasAPIIFMock& apiMock = dynamic_cast<WasAPIIFMock&>(*(pAPI.data()));
    EXPECT_CALL(apiMock,isExclusive()).WillRepeatedly(Return(false));

    FormatDescription sourceDesc;

    IAudioClientIFSPtr pAudioClient(new IAudioClientIFMock());
    IAudioClientIFMock& mockAudio = dynamic_cast<IAudioClientIFMock&>(*(pAudioClient.data()));

    WasAPIDeviceLayerFindClosestSupportedFormatTest deviceLayer(pAudioClient);

    WAVEFORMATEX *formatA,*formatClosestA;
    formatA = new WAVEFORMATEX;
    deviceLayer.testDefaultWaveFormat(*formatA);
    deviceLayer.testSetWaveFormat(2,16,44100,*formatA);
    formatA->cbSize = 22;
    
    formatClosestA = CoTaskMemAlloc(sizeof(WAVEFORMATEX));
    ASSERT_TRUE(formatClosestA!=0);
    deviceLayer.testDefaultWaveFormat(*formatClosestA);
    deviceLayer.testSetWaveFormat(2,24,44100,*formatClosestA);
        
    EXPECT_CALL(deviceLayer,findClosestWaveFormatFromDescription(sourceDesc)).Times(1).WillOnce(Return(formatA));
    
    EXPECT_CALL(mockAudio,IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,WaveFormatExIsExpected(formatA),A<WAVEFORMATEX **>()))
        .Times(1).WillOnce(DoAll(SetArgPointee<2>(formatClosestA),Return(S_FALSE)));
        
    WAVEFORMATEX *pActualFormat = deviceLayer.findClosestSupportedFormat(sourceDesc);
    
    ASSERT_TRUE(pActualFormat==0);
    
    WasAPIIF::release();
}


TEST(WasAPIDeviceLayer,findClosestSupportedFormatWithSharedAndNonExIsNotSupportedAndExtIsSupported)
{
    WasAPIIF::setExclusive(false);
    WasAPIIFSPtr pAPI = WasAPIIF::instance("mock");
    WasAPIIFMock& apiMock = dynamic_cast<WasAPIIFMock&>(*(pAPI.data()));
    EXPECT_CALL(apiMock,isExclusive()).WillRepeatedly(Return(false));

    FormatDescription sourceDesc;

    IAudioClientIFSPtr pAudioClient(new IAudioClientIFMock());
    IAudioClientIFMock& mockAudio = dynamic_cast<IAudioClientIFMock&>(*(pAudioClient.data()));

    WasAPIDeviceLayerFindClosestSupportedFormatTest deviceLayer(pAudioClient);

    WAVEFORMATEX *formatA,*formatClosestA;
    formatA = new WAVEFORMATEX;
    deviceLayer.testDefaultWaveFormat(*formatA);
    deviceLayer.testSetWaveFormat(2,16,44100,*formatA);
    WAVEFORMATEXTENSIBLE *formatExtA = deviceLayer.toWaveExtensible(formatA);
    
    formatClosestA = CoTaskMemAlloc(sizeof(WAVEFORMATEX));
    ASSERT_TRUE(formatClosestA!=0);
    deviceLayer.testDefaultWaveFormat(*formatClosestA);
    deviceLayer.testSetWaveFormat(2,24,44100,*formatClosestA);

    WAVEFORMATEXTENSIBLE *formatClosestExtB = deviceLayer.toWaveExtensible(&formatClosestA);
    WAVEFORMATEXTENSIBLE *formatClosestExtA = CoTaskMemAlloc(sizeof(WAVEFORMATEXTENSIBLE));
    ASSERT_TRUE(formatClosestExtA!=0);
    memcpy(formatClosestExtA,formatClosestExtB,sizeof(WAVEFORMATEXTENSIBLE));
    delete formatClosestExtB;
    
    EXPECT_CALL(deviceLayer,findClosestWaveFormatFromDescription(sourceDesc)).Times(1).WillOnce(Return(formatA));
    
    EXPECT_CALL(mockAudio,IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,WaveFormatExIsExpected(formatA),A<WAVEFORMATEX **>()))
        .Times(1).WillOnce(DoAll(SetArgPointee<2>(formatClosestA),Return(S_FALSE)));
    EXPECT_CALL(mockAudio,IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,WaveFormatExtensibleIsExpected(formatExtA),A<WAVEFORMATEX **>()))
        .Times(1).WillOnce(DoAll(SetArgPointee<2>(formatClosestExtA),Return(S_OK)));
    
    WAVEFORMATEX *pActualFormat = deviceLayer.findClosestSupportedFormat(sourceDesc);
    
    ASSERT_TRUE(pActualFormat!=0);
    EXPECT_EQ(0,memcmp(pActualFormat,formatExtA,sizeof(WAVEFORMATEXTENSIBLE)));
    
    delete pActualFormat;
    delete formatExtA;
    
    WasAPIIF::release();
}

TEST(WasAPIDeviceLayer,findClosestSupportedFormatWithSharedAndNonExAndExtIsNotSupported)
{
    WasAPIIF::setExclusive(false);
    WasAPIIFSPtr pAPI = WasAPIIF::instance("mock");
    WasAPIIFMock& apiMock = dynamic_cast<WasAPIIFMock&>(*(pAPI.data()));
    EXPECT_CALL(apiMock,isExclusive()).WillRepeatedly(Return(false));

    FormatDescription sourceDesc;

    IAudioClientIFSPtr pAudioClient(new IAudioClientIFMock());
    IAudioClientIFMock& mockAudio = dynamic_cast<IAudioClientIFMock&>(*(pAudioClient.data()));

    WasAPIDeviceLayerFindClosestSupportedFormatTest deviceLayer(pAudioClient);

    WAVEFORMATEX *formatA,*formatClosestA;
    formatA = new WAVEFORMATEX;
    deviceLayer.testDefaultWaveFormat(*formatA);
    deviceLayer.testSetWaveFormat(2,16,44100,*formatA);
    WAVEFORMATEXTENSIBLE *formatExtA = deviceLayer.toWaveExtensible(formatA);
    
    formatClosestA = CoTaskMemAlloc(sizeof(WAVEFORMATEX));
    ASSERT_TRUE(formatClosestA!=0);
    deviceLayer.testDefaultWaveFormat(*formatClosestA);
    deviceLayer.testSetWaveFormat(2,24,44100,*formatClosestA);

    WAVEFORMATEXTENSIBLE *formatClosestExtB = deviceLayer.toWaveExtensible(&formatClosestA);
    WAVEFORMATEXTENSIBLE *formatClosestExtA = CoTaskMemAlloc(sizeof(WAVEFORMATEXTENSIBLE));
    ASSERT_TRUE(formatClosestExtA!=0);
    memcpy(formatClosestExtA,formatClosestExtB,sizeof(WAVEFORMATEXTENSIBLE));
    delete formatClosestExtB;
    
    EXPECT_CALL(deviceLayer,findClosestWaveFormatFromDescription(sourceDesc)).Times(1).WillOnce(Return(formatA));
    
    EXPECT_CALL(mockAudio,IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,WaveFormatExIsExpected(formatA),A<WAVEFORMATEX **>()))
        .Times(1).WillOnce(DoAll(SetArgPointee<2>(formatClosestA),Return(S_FALSE)));
    EXPECT_CALL(mockAudio,IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,WaveFormatExtensibleIsExpected(formatExtA),A<WAVEFORMATEX **>()))
        .Times(1).WillOnce(DoAll(SetArgPointee<2>(formatClosestExtA),Return(S_FALSE)));
    
    WAVEFORMATEX *pActualFormat = deviceLayer.findClosestSupportedFormat(sourceDesc);
    
    EXPECT_TRUE(pActualFormat==0);
    
    delete formatExtA;
    
    WasAPIIF::release();
}
