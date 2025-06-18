#include "audioio/test/integration/ALSATest.h"

//-------------------------------------------------------------------------------------------

TEST(ALSATest,queryDevicesExclusive)
{
    LinuxALSAIF::instance("alsa");

    AOQueryALSA audioQuery;
    
    ASSERT_TRUE(audioQuery.queryNames());

    for(int i=0;i<audioQuery.noDevices();i++)
    {
        ASSERT_TRUE(audioQuery.queryDevice(i));
    }
    
    audioQuery.print();
        
    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------
// ALSAPlaybackTester
//-------------------------------------------------------------------------------------------

ALSAPlaybackTester::ALSAPlaybackTester(int& argc,char **argv) : QCoreApplication(argc,argv),
    m_handle(0),
    m_hwParams(0),
    m_swParams(0),
    m_frequency(48000),
    m_noSamplesInPeriod(0),
    m_noSamplesInBuffer(0),
    m_phase(0.0),
    m_frequencySignal(500),
    m_audioWriteBuffer(0)
{
    QTimer::singleShot(10,this,SLOT(onStart()));
}

//-------------------------------------------------------------------------------------------

ALSAPlaybackTester::~ALSAPlaybackTester()
{}

//-------------------------------------------------------------------------------------------

void ALSAPlaybackTester::printError(const tchar *strR,const tchar *strE) const
{
    fprintf(stdout,"ALSAPlaybackTester::%s - %s.\n",strR,strE);
}

//-------------------------------------------------------------------------------------------

void ALSAPlaybackTester::printErrorOS(const tchar *strR,const tchar *strE,int errAlsa) const
{
    QString err = QString::fromLatin1(strE) + ". " + QString::fromUtf8(snd_strerror(errAlsa));
    printError(strR,err.toUtf8().constData());
}

//-------------------------------------------------------------------------------------------

bool ALSAPlaybackTester::openAudio()
{
    int status;
    bool res = false;
    
    closeAudio();
    
    status = snd_pcm_open(&m_handle,"hw:0",SND_PCM_STREAM_PLAYBACK,0);
    if(!status)
    {
        if(setupHardwareParameters())
        {
            if(setupSoftwareParameters())
            {
                res = true;
            }
            else
            {
                printError("openAudio","Failed to set software configuration");
            }
        }
        else
        {
            printError("openAudio","Failed to set hardware configuration");
        }
    }
    else
    {
        printErrorOS("openAudio","Failed to open PCM audio for default card",status);
    }
    
    if(!res)
    {
        closeAudio();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void ALSAPlaybackTester::closeAudio()
{
    int status;

    if(m_audioWriteBuffer!=0)
    {
        delete [] m_audioWriteBuffer;
        m_audioWriteBuffer = 0;
    }
    if(m_hwParams!=0)
    {
        snd_pcm_hw_params_free(m_hwParams);
        m_hwParams = 0;
    }
    if(m_swParams!=0)
    {
        snd_pcm_sw_params_free(m_swParams);
        m_swParams = 0;
    }
    if(m_handle!=0)
    {
        status = snd_pcm_close(m_handle);
        if(status)
        {
            printErrorOS("closeAudio","Error closing PCM audio for card",status);
        }
        m_handle = 0;
    }
}

//-------------------------------------------------------------------------------------------

bool ALSAPlaybackTester::setupHardwareParameters()
{
    int status;
    bool res = false;
    
    status = snd_pcm_hw_params_malloc(&m_hwParams);
    if(!status && m_hwParams!=0)
    {
        status = snd_pcm_hw_params_any(m_handle,m_hwParams);
        if(!status)
        {
            status = snd_pcm_hw_params_set_rate_resample(m_handle,m_hwParams,0);
            if(!status)
            {
                status = snd_pcm_hw_params_set_access(m_handle,m_hwParams,SND_PCM_ACCESS_RW_INTERLEAVED);
                if(!status)
                {
                    status = snd_pcm_hw_params_set_channels(m_handle,m_hwParams,2);
                    if(!status)
                    {
                        snd_pcm_format_t formatType = SND_PCM_FORMAT_S16_LE;
                        
                        status = snd_pcm_hw_params_set_format(m_handle,m_hwParams,formatType);
                        if(!status)
                        {
                            status = snd_pcm_hw_params_set_rate(m_handle,m_hwParams,m_frequency,0);
                            if(!status)
                            {
                                if(setBufferLength())
                                {
                                    status = snd_pcm_hw_params(m_handle,m_hwParams);
                                    if(!status)
                                    {
                                        res = true;
                                    }
                                    else
                                    {
                                        printErrorOS("setupHardwareParameters","Failed to set hardware configuration for playback",status);
                                    }
                                }
                                else
                                {
                                    printErrorOS("setupHardwareParameters","Failed to setup buffer length",status);
                                }
                            }
                            else
                            {
                                printErrorOS("setupHardwareParameters","Failed to set frequency of playback",status);
                            }
                        }
                        else
                        {
                            printErrorOS("setupHardwareParameters","Failed to set format to 16-bit signed",status);
                        }
                    }
                    else
                    {
                        printErrorOS("setupHardwareParameters","Failed to set number of channels to stereo",status);
                    }
                }
                else
                {
                    printErrorOS("setupHardwareParameters","Failed to setup parameter access",status);
                }
            }
            else
            {
                printErrorOS("setupHardwareParameters","Error turning of software resampler",status);
            }
        }
        else
        {
            printErrorOS("setupHardwareParameters","Error getting hardware configuration parameters",status);
        }
    }
    else
    {
        printErrorOS("setupHardwareParameters","Failed to allocate memory for hardware parameters",status);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool ALSAPlaybackTester::setBufferLength()
{
    int status;
    tuint maxBufferTime = 0;
    bool res = false;
    
    status = snd_pcm_hw_params_get_buffer_time_max(m_hwParams,&maxBufferTime,0);
    if(!status)
    {
        tuint bufferTime = (maxBufferTime < 200000) ? maxBufferTime : 200000;
        tuint periodTime = bufferTime / 4;
        
        status = snd_pcm_hw_params_set_period_time_near(m_handle,m_hwParams,&periodTime,0);
        if(!status)
        {
            status = snd_pcm_hw_params_set_buffer_time_near(m_handle,m_hwParams,&bufferTime,0);
            if(!status)
            {
                res = true;
            }
            else
            {
                printErrorOS("setBufferLength","Error setting audio buffer time length",status);
            }
        }
        else
        {
            printErrorOS("setBufferLength","Error setting period time length",status);
        }
    }
    else
    {
        printErrorOS("setBufferLength","Failed to get maximum buffer time",status);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool ALSAPlaybackTester::setupSoftwareParameters()
{
    int status;
    bool res = false;

    status = snd_pcm_sw_params_malloc(&m_swParams);
    if(!status)
    {
        if(queryBufferSize())
        {
            status = snd_pcm_sw_params_current(m_handle,m_swParams);
            if(!status)
            {
                status = snd_pcm_sw_params_set_start_threshold(m_handle,m_swParams,m_noSamplesInBuffer - m_noSamplesInPeriod);
                if(!status)
                {
                    status = snd_pcm_sw_params_set_avail_min(m_handle,m_swParams,m_noSamplesInPeriod);
                    if(!status)
                    {
                        status = snd_pcm_sw_params_set_tstamp_mode(m_handle,m_swParams,SND_PCM_TSTAMP_ENABLE);
                        if(!status)
                        {
                            status = snd_pcm_sw_params(m_handle,m_swParams);
                            if(!status)
                            {
                                res = true;
                            }
                            else
                            {
                                printErrorOS("setupSoftwareParameters","Error setting software configuration",status);
                            }
                        }
                        else
                        {
                            printErrorOS("setupSoftwareParameters","Failed to enable timestamp update",status);
                        }
                    }
                    else
                    {
                        printErrorOS("setupSoftwareParameters","Failed to set minimum available audio",status);
                    }
                }
                else
                {
                    printErrorOS("setupSoftwareParameters","Failed to set start audio threshold",status);
                }
            }
            else
            {
                printErrorOS("setupSoftwareParameters","Error getting current software parameters",status);
            }
        }
        else
        {
            printError("setupSoftwareParameters","Failed to get period and buffer size");
        }
    }
    else
    {
        printErrorOS("setupSoftwareParameters","Failed to allocate memory software parameters",status);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool ALSAPlaybackTester::queryBufferSize()
{
    int status;
    snd_pcm_uframes_t periodSize,bufferSize;
    bool res = false;
    
    status = snd_pcm_hw_params_get_period_size(m_hwParams,&periodSize,0);
    if(!status)
    {
        status = snd_pcm_hw_params_get_buffer_size(m_hwParams,&bufferSize);
        if(!status)
        {
            m_noSamplesInPeriod = periodSize;
            m_noSamplesInBuffer = bufferSize;
            m_audioWriteBuffer = new tint16 [m_noSamplesInBuffer * 2];
            res = true;
        }
        else
        {
            printErrorOS("queryBufferSize","Failed to get number of samples in audio buffer",status);
        }
    }
    else
    {
        printErrorOS("queryBufferSize","Failed to get number of samples in period",status);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void ALSAPlaybackTester::writeAudio(snd_async_handler_t *pCallback)
{
    ALSAPlaybackTester *pPlayer = reinterpret_cast<ALSAPlaybackTester *>(snd_async_handler_get_callback_private(pCallback));
    pPlayer->writeAudioImpl(pCallback);
}

//-------------------------------------------------------------------------------------------

void ALSAPlaybackTester::writeAudioImpl(snd_async_handler_t *pCallback)
{
    tint avail;
    snd_pcm_t *handle = snd_async_handler_get_pcm(pCallback);

    {
        int status;
        snd_pcm_uframes_t tAvail = 0;
        snd_htimestamp_t tTimeStamp;
        
        status = snd_pcm_htimestamp(handle,&tAvail,&tTimeStamp);
        if(!status)
        {
            common::TimeStamp tS;
            tS.set(&tTimeStamp);
            fprintf(stdout,"TimeStmp - %.8f\n",static_cast<tfloat64>(tS));
        }
    }

    avail = static_cast<tint>(snd_pcm_avail_update(handle));
    while(avail >= m_noSamplesInPeriod)
    {
        generateAndWriteAudio(handle,m_noSamplesInPeriod);
        avail = static_cast<tint>(snd_pcm_avail_update(handle));
    }    
}

//-------------------------------------------------------------------------------------------

void ALSAPlaybackTester::generateAndWriteAudio(snd_pcm_t *handle,tint len)
{
    tint noChannels = 2;
    tfloat64 phase = m_phase;
    tfloat64 phaseInc = (2.0 * c_PI_D * static_cast<tfloat64>(m_frequencySignal)) / static_cast<tfloat64>(m_frequency);

    for(tint i=0;i<len;i++,phase+=phaseInc)
    {
        tfloat64 v = sin(phase);
        tint16 *x = &m_audioWriteBuffer[i * noChannels];
        
        for(tint j=0;j<noChannels;j++)
        {
            engine::write16BitsLittleEndianFromSample(v,reinterpret_cast<tchar *>(&x[j]));
        }
    }
    snd_pcm_writei(handle,m_audioWriteBuffer,len);
    m_phase = phase;
}

//-------------------------------------------------------------------------------------------

bool ALSAPlaybackTester::startAudio()
{
    int status;
    bool res = false;

    status = snd_async_add_pcm_handler(&m_pCallback,m_handle,ALSAPlaybackTester::writeAudio,this);
    if(!status)
    {
        status = snd_pcm_prepare(m_handle);
        if(!status)
        {
            generateAndWriteAudio(m_handle,m_noSamplesInBuffer);
            res = true;
        }
        else
        {
            printErrorOS("startAudio","Failed to prepare PCM output",status);
        }
    }
    else
    {
        printErrorOS("startAudio","Failed to attach callback",status);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void ALSAPlaybackTester::stopAudio()
{
    snd_pcm_drop(m_handle);
    
    if(m_pCallback!=0)
    {
        snd_async_del_handler(m_pCallback);
        m_pCallback = 0;
    }
}

//-------------------------------------------------------------------------------------------

void ALSAPlaybackTester::onStart()
{
    bool res = false;
    
    if(openAudio())
    {
        if(startAudio())
        {
            QTimer::singleShot(10000,this,SLOT(onStop()));
            res = true;
        }
    }
    if(!res)
    {
        onStop();
    }
}

//-------------------------------------------------------------------------------------------

void ALSAPlaybackTester::onStop()
{
    stopAudio();
    closeAudio();
    quit();
}

//-------------------------------------------------------------------------------------------

TEST(ALSATest,playWithCallback)
{
    int argc = 0;
    ALSAPlaybackTester signalApp(argc,0);
    signalApp.exec();
}

//-------------------------------------------------------------------------------------------
