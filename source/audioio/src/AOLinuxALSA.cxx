//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOLinuxALSA.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------


CONCRETE_FACTORY_CLASS_IMPL(AOFactory,AOBase, \
                            AOLinuxALSAFactory,AOLinuxALSA, \
                            "alsa",false)

//-------------------------------------------------------------------------------------------

AOLinuxALSA::AOLinuxALSA(QObject *parent) : AOBase(parent),
	m_handleALSA(0),
	m_hwParamsALSA(0),
	m_swParamsALSA(0),
	m_formatTypeALSA(SND_PCM_FORMAT_UNKNOWN),
	m_noSamplesInPeriodALSA(0),
	m_noSamplesInBufferALSA(0),
	m_pCallback(0),
	m_pSampleConverter(0),
	m_flagInit(false),
	m_flagStart(false),
	m_playbackALSAMemoryBuffers(),
	m_playbackALSAMemoryBufferSize(0)
{}

//-------------------------------------------------------------------------------------------

AOLinuxALSA::~AOLinuxALSA()
{
	AOLinuxALSA::freeALSAPlaybackBuffers();
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::printError(const tchar *strR,const tchar *strE) const
{
	common::Log::g_Log << "AOLinuxALSA::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::printErrorOS(const tchar *strR,const tchar *strE,tint errCode) const
{
	QString err = QString::fromLatin1(strE) + ". " + QString::fromUtf8(LinuxALSAIF::instance()->snd_strerror(errCode));
	printError(strR,err.toUtf8().constData());
}

//-------------------------------------------------------------------------------------------

engine::AData *AOLinuxALSA::allocateData(tint len,tint inChannel,tint outChannel)
{
    engine::RData *d = new engine::RData(len,inChannel,outChannel);
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::allocateData\n");
#endif
	return reinterpret_cast<engine::AData *>(d);
}

//-------------------------------------------------------------------------------------------

bool AOLinuxALSA::init()
{
	LinuxALSAIF::instance("alsa");
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::init\n");
#endif
	return AOBase::init();
}

//-------------------------------------------------------------------------------------------

bool AOLinuxALSA::openAudio()
{
	int status;
	bool res = false;
	
	closeAudio();
	m_frequency = m_codec->frequency();

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::openAudio\n");
#endif

	QSharedPointer<AOQueryALSA::DeviceALSA> pDeviceALSA = getCurrentALSAAudioDevice();
	if(pDeviceALSA.isNull())
	{
		printError("openAudio","Could not find audio device");
		return false;
	}
	
	QString hwName = pDeviceALSA->pcmDeviceName().toLatin1();
	
    status = LinuxALSAIF::instance()->snd_pcm_open(&m_handleALSA,hwName.toLatin1().data(),SND_PCM_STREAM_PLAYBACK,0);
    if(!status)
    {
		FormatDescription closestDescription;
		FormatDescription sourceDescription = getSourceDescription(pDeviceALSA->noChannels());

		if(FormatDescriptionUtils::findClosestFormatType(sourceDescription,pDeviceALSA->supported(),closestDescription))
		{
			if(m_forceBitsPerSample > 0)
			{
				FormatDescription cDescription(FormatDescription::e_DataSignedInteger,m_forceBitsPerSample,pDeviceALSA->noChannels(),getFrequency());
				closestDescription = cDescription;
			}

			int formatType = formatFromDescription(m_handleALSA,pDeviceALSA,closestDescription);
			
			if(formatType!=SND_PCM_FORMAT_UNKNOWN)
			{
				if(setupHardwareParameters(formatType,closestDescription))
				{
					if(setupSoftwareParameters())
					{
						m_pSampleConverter = createSampleConverter(formatType);
						if(m_pSampleConverter!=0)
						{
							setResamplerAsRequired(m_codec->frequency(),closestDescription.frequency());
							initCyclicBuffer();
							m_formatTypeALSA = formatType;
							m_noChannels = closestDescription.channels();
							allocALSAPlaybackBuffers(m_formatTypeALSA, m_noChannels);
							m_flagInit = true;
							res = true;
						}
						else
						{
							printError("openAudio","Failed to create sample convertion unit");
						}
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
				printError("openAudio","Could not find supported byte stream format for codec");
			}
		}
		else
		{
			printError("openAudio","Could not find supported format DAC description for codec");
		}    
    }
    else
    {
    	QString err = "Unable to open PCM device '" + hwName + "' for playback";
    	printErrorOS("openAudio",err.toUtf8().constData(),status);
    }
    	
	return res;
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::closeAudio()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::closeAudio\n");
#endif

	stopAudioDevice();
	closeResampler();
	
	if(m_pSampleConverter!=0)
	{
		delete m_pSampleConverter;
		m_pSampleConverter = 0;
	}
	if(m_swParamsALSA!=0)
	{
		LinuxALSAIF::instance()->snd_pcm_sw_params_free(m_swParamsALSA);
		m_swParamsALSA = 0;
	}
	if(m_hwParamsALSA!=0)
	{
		LinuxALSAIF::instance()->snd_pcm_hw_params_free(m_hwParamsALSA);
		m_hwParamsALSA = 0;
	}
	if(m_handleALSA!=0)
	{
        int status = LinuxALSAIF::instance()->snd_pcm_close(m_handleALSA);
		if(status)
		{
			printErrorOS("closeAudio","Error closing PCM audio for card",status);
		}
		m_handleALSA = 0;
	}
	freeALSAPlaybackBuffers();
	m_flagInit = false;
}

//-------------------------------------------------------------------------------------------

bool AOLinuxALSA::startAudioDevice()
{
	bool res = false;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::startAudioDevice\n");
#endif

	if(m_flagInit)
	{
		int status;
		
		status = LinuxALSAIF::instance()->snd_async_add_pcm_handler(&m_pCallback,m_handleALSA,AOLinuxALSA::writeAudioALSA,this);
		if(!status)
		{
			status = LinuxALSAIF::instance()->snd_pcm_prepare(m_handleALSA);
			if(!status)
			{
                writeAudioToALSA(m_handleALSA,m_noSamplesInBufferALSA);
				m_flagStart = true;
				res = true;
			}
			else
			{
				printErrorOS("startAudioDevice","Failed to prepare PCM output for playback",status);
			}
		}
		else
		{
			printErrorOS("startAudioDevice","Failed to attach asynchronous callback",status);
		}
	}
	else
	{
		printError("startAudioDevice","Audio output has not been opened");
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::stopAudioDevice()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::stopAudioDevice\n");
#endif

	if(m_flagInit)
	{
		if(m_flagStart)
		{
			LinuxALSAIF::instance()->snd_pcm_drop(m_handleALSA);
			m_flagStart = false;
		}
		if(m_pCallback!=0)
		{
			LinuxALSAIF::instance()->snd_async_del_handler(m_pCallback);
			m_pCallback = 0;
		}
	}
}

//-------------------------------------------------------------------------------------------

bool AOLinuxALSA::isAudio() const
{
	return m_flagInit;
}

//-------------------------------------------------------------------------------------------

int AOLinuxALSA::formatFromDescription(snd_pcm_t *handle,QSharedPointer<AOQueryALSA::DeviceALSA>& pDeviceALSA,const FormatDescription& desc)
{
	int format = SND_PCM_FORMAT_UNKNOWN;
	QVector<int> formats = pDeviceALSA->formatsFromDescription(desc);

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::formatFromDescription\n");
#endif

	if(!formats.isEmpty())
	{
		if(formats.size() > 1)
		{
			for(QVector<int>::iterator ppI=formats.begin();ppI!=formats.end() && format==SND_PCM_FORMAT_UNKNOWN;ppI++)
			{
				if(pDeviceALSA->canSupportFormat(handle,*ppI,desc.channels(),desc.frequency()))
				{
					format = *ppI;
				}
			}
		}
		else
		{
			format = formats.at(0);
		}
	}
	return format;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<AOQueryALSA::DeviceALSA> AOLinuxALSA::getCurrentALSAAudioDevice()
{
	QSharedPointer<AOQueryALSA::DeviceALSA> pDeviceALSA;
	QSharedPointer<AOQueryDevice::Device> pDevice = getCurrentDevice();
	if(!pDevice.isNull())
	{
		pDeviceALSA = pDevice.dynamicCast<AOQueryALSA::DeviceALSA>();
	}
	return pDeviceALSA;	
}

//-------------------------------------------------------------------------------------------

bool AOLinuxALSA::setupHardwareParameters(int fType,const FormatDescription& desc)
{
	int status;
	bool res = false;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::setupHardwareParameters\n");
#endif

	status = LinuxALSAIF::instance()->snd_pcm_hw_params_malloc(&m_hwParamsALSA);
    if(!status && m_hwParamsALSA!=0)
	{
        status = LinuxALSAIF::instance()->snd_pcm_hw_params_any(m_handleALSA,m_hwParamsALSA);
		if(!status)
		{
            status = LinuxALSAIF::instance()->snd_pcm_hw_params_set_rate_resample(m_handleALSA,m_hwParamsALSA,0);
			if(!status)
			{
                status = LinuxALSAIF::instance()->snd_pcm_hw_params_set_access(m_handleALSA,m_hwParamsALSA,SND_PCM_ACCESS_RW_INTERLEAVED);
				if(!status)
				{
                    status = LinuxALSAIF::instance()->snd_pcm_hw_params_set_channels(m_handleALSA,m_hwParamsALSA,desc.channels());
					if(!status)
					{
                        snd_pcm_format_t formatType = static_cast<snd_pcm_format_t>(fType);
						
                        status = LinuxALSAIF::instance()->snd_pcm_hw_params_set_format(m_handleALSA,m_hwParamsALSA,formatType);
						if(!status)
						{
                            status = LinuxALSAIF::instance()->snd_pcm_hw_params_set_rate(m_handleALSA,m_hwParamsALSA,desc.frequency(),0);
							if(!status)
							{
								if(setBufferLength())
								{
                                    status = LinuxALSAIF::instance()->snd_pcm_hw_params(m_handleALSA,m_hwParamsALSA);
									if(!status)
									{
                                        QString fStr = AOQueryALSA::DeviceALSA::formatToString(fType) + " Freq=" + QString::number(desc.frequency());
                                        fStr += "Hz Channels=" + QString::number(desc.channels());
										fprintf(stdout,"%s\n",fStr.toLatin1().constData());
										
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
							printErrorOS("setupHardwareParameters","Failed to set byte format",status);
						}
					}
					else
					{
						printErrorOS("setupHardwareParameters","Failed to set number of channels",status);
					}
				}
				else
				{
					printErrorOS("setupHardwareParameters","Failed to setup interleaved parameter access",status);
				}
			}
			else
			{
				printErrorOS("setupHardwareParameters","Error turning of software resampler",status);
			}
		}
		else
		{
			printErrorOS("setupHardwareParameters","Error getting hardware configuration parameters for audio device",status);
		}
	}
	else
	{
		printErrorOS("setupHardwareParameters","Failed to allocate memory for ALSA hardware parameters",status);
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOLinuxALSA::setBufferLength()
{
	int status;
	tuint maxBufferTime = 0;
	bool res = false;
	
    status = LinuxALSAIF::instance()->snd_pcm_hw_params_get_buffer_time_max(m_hwParamsALSA,&maxBufferTime,0);
	if(!status)
	{
		tuint bufferTime = (maxBufferTime < 500000) ? maxBufferTime : 500000;
		tuint periodTime = bufferTime / 4;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::setBufferLength - %d, %d\n", bufferTime, periodTime);
#endif
		
		status = LinuxALSAIF::instance()->snd_pcm_hw_params_set_period_time_near(m_handleALSA,m_hwParamsALSA,&periodTime,0);
		if(!status)
		{
            status = LinuxALSAIF::instance()->snd_pcm_hw_params_set_buffer_time_near(m_handleALSA,m_hwParamsALSA,&bufferTime,0);
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

bool AOLinuxALSA::queryBufferSize()
{
	int status;
	snd_pcm_uframes_t periodSize,bufferSize;
	bool res = false;
	
	status = LinuxALSAIF::instance()->snd_pcm_hw_params_get_period_size(m_hwParamsALSA,&periodSize,0);
	if(!status)
	{
        status = LinuxALSAIF::instance()->snd_pcm_hw_params_get_buffer_size(m_hwParamsALSA,&bufferSize);
		if(!status)
		{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::queryBufferSize - %d, %d\n", periodSize, bufferSize);
#endif
            m_noSamplesInPeriodALSA = periodSize;
            m_noSamplesInBufferALSA = bufferSize;
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

bool AOLinuxALSA::setupSoftwareParameters()
{
	int status;
	bool res = false;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::setupSoftwareParameters\n");
#endif

    status = LinuxALSAIF::instance()->snd_pcm_sw_params_malloc(&m_swParamsALSA);
	if(!status)
	{
		if(queryBufferSize())
		{
            status = LinuxALSAIF::instance()->snd_pcm_sw_params_current(m_handleALSA,m_swParamsALSA);
			if(!status)
			{
                status = LinuxALSAIF::instance()->snd_pcm_sw_params_set_start_threshold(m_handleALSA,m_swParamsALSA,m_noSamplesInBufferALSA - m_noSamplesInPeriodALSA);
				if(!status)
				{
                    status = LinuxALSAIF::instance()->snd_pcm_sw_params_set_avail_min(m_handleALSA,m_swParamsALSA,m_noSamplesInPeriodALSA);
					if(!status)
					{
                        status = LinuxALSAIF::instance()->snd_pcm_sw_params_set_tstamp_mode(m_handleALSA,m_swParamsALSA,SND_PCM_TSTAMP_ENABLE);
						if(!status)
                        {
                            status = LinuxALSAIF::instance()->snd_pcm_sw_params(m_handleALSA,m_swParamsALSA);
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

SampleConverter *AOLinuxALSA::createSampleConverter(tint formatType)
{
	SampleConverter *sampleConverter;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::createSampleConverter - %d\n", formatType);
#endif

	switch(formatType)
	{
        case SND_PCM_FORMAT_S8:
			sampleConverter = new SampleConverter(8,1,true,false,true);
			break;
		case SND_PCM_FORMAT_S16_LE:
			sampleConverter = new SampleConverter(16,2,true,false,true);
			break;
		case SND_PCM_FORMAT_S16_BE:
			sampleConverter = new SampleConverter(16,2,false,false,true);
			break;
		case SND_PCM_FORMAT_S24_LE:
			sampleConverter = new SampleConverter(24,4,true,false,true);
			break;
		case SND_PCM_FORMAT_S24_BE:
			sampleConverter = new SampleConverter(24,4,false,false,true);
			break;
		case SND_PCM_FORMAT_S32_LE:
			sampleConverter = new SampleConverter(32,4,true,false,true);
			break;
		case SND_PCM_FORMAT_S32_BE:
			sampleConverter = new SampleConverter(32,4,false,false,true);
			break;
		case SND_PCM_FORMAT_FLOAT_LE:
			sampleConverter = new SampleConverter(true,true);
			break;
		case SND_PCM_FORMAT_FLOAT_BE:
			sampleConverter = new SampleConverter(true,false);
			break;
		case SND_PCM_FORMAT_FLOAT64_LE:
			sampleConverter = new SampleConverter(false,true);
			break;
		case SND_PCM_FORMAT_FLOAT64_BE:
			sampleConverter = new SampleConverter(false,false);
			break;
		case SND_PCM_FORMAT_S24_3LE:
			sampleConverter = new SampleConverter(24,3,true,false,true);
			break;
		case SND_PCM_FORMAT_S24_3BE:
			sampleConverter = new SampleConverter(24,3,false,false,true);
			break;
		case SND_PCM_FORMAT_S20_3LE:
			sampleConverter = new SampleConverter(20,3,true,false,true);
			break;
		case SND_PCM_FORMAT_S20_3BE:
			sampleConverter = new SampleConverter(20,3,false,false,true);
			break;
		case SND_PCM_FORMAT_S18_3LE:
			sampleConverter = new SampleConverter(18,3,true,false,true);
			break;
		case SND_PCM_FORMAT_S18_3BE:
			sampleConverter = new SampleConverter(18,3,false,false,true);
			break;
		default:
			sampleConverter = 0;
			break;
	}
	return sampleConverter;
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::setResamplerAsRequired(tint codecFrequency,tint deviceFrequency)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::setResamplerAsRequired - %d\n", deviceFrequency);
#endif

	if(codecFrequency!=deviceFrequency)
	{
		initResampler(codecFrequency,deviceFrequency);
	}
	m_frequency = deviceFrequency;
}

//-------------------------------------------------------------------------------------------

SampleConverter *AOLinuxALSA::getSampleConverter()
{
	return m_pSampleConverter;
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::writeAudioALSA(snd_async_handler_t *pCallback)
{
	AOLinuxALSA *pAudio = reinterpret_cast<AOLinuxALSA *>(LinuxALSAIF::instance()->snd_async_handler_get_callback_private(pCallback));
	pAudio->writeAudioALSAImpl(pCallback);
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::writeAudioALSAImpl(snd_async_handler_t *pCallback)
{
	bool loop = true;
	snd_pcm_t *handle = LinuxALSAIF::instance()->snd_async_handler_get_pcm(pCallback);

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::writeAudioALSAImpl\n");
#endif

	while(loop)
	{
		tint avail = LinuxALSAIF::instance()->snd_pcm_avail_update(handle);
        if(avail >= m_noSamplesInPeriodALSA)
		{
			writeAudioToALSA(handle,avail);
		}
		else
		{
			loop = false;
		}
	}
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::writeAudioToALSA(snd_pcm_t *handle,tint noFrames)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::writeAudioToALSA\n");
#endif
	if(!m_playbackALSAMemoryBuffers.isEmpty())
	{
		tbyte *pBuffer = m_playbackALSAMemoryBuffers.dequeue();
		AudioHardwareBufferALSA buffer(m_formatTypeALSA, noFrames, m_noChannels, pBuffer, m_playbackALSAMemoryBufferSize);
		IOTimeStamp systemTime = createIOTimeStamp(handle);
		writeToAudioIOCallback(&buffer,systemTime);
		LinuxALSAIF::instance()->snd_pcm_writei(handle,buffer.buffer(0),noFrames);
		m_playbackALSAMemoryBuffers.enqueue(pBuffer);
	}
	else
	{
		printError("writeAudioToALSA","No direct audio buffers available");	
	}
}

//-------------------------------------------------------------------------------------------

IOTimeStamp AOLinuxALSA::createIOTimeStamp(snd_pcm_t *handle) const
{
	int status;
	snd_pcm_uframes_t tAvail = 0;
	snd_htimestamp_t tTimeStamp;
	common::TimeStamp tS;
	bool valid = false;
	
	status = LinuxALSAIF::instance()->snd_pcm_htimestamp(handle,&tAvail,&tTimeStamp);
	if(!status && (tTimeStamp.tv_sec || tTimeStamp.tv_nsec))
	{
		tS.set(&tTimeStamp);
		valid = true;
	}
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::createIOTimeStamp - %.8f, %d\n", static_cast<tfloat64>(tS), valid);
#endif
	return IOTimeStamp(valid,tS);
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::writeToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,
                                                		const engine::RData *data,
                                                    	tint partNumber,
                                                        tint inChannelIndex,
                                                        tint bufferIndex,
                                                        tint outChannelIndex,
                                                        tint inputSampleIndex,
                                                        tint outputSampleIndex,
                                                        tint amount)

{
	const sample_t *input;
	
	engine::CodecDataType dType;
	tint noInputChannels;
	tint noOutputChannels = pBuffer->numberOfChannelsInBuffer(bufferIndex);
	tint iIdx;
	tint oIdx = (outputSampleIndex * noOutputChannels) + outChannelIndex;
	
	tbyte *out = reinterpret_cast<tbyte *>(pBuffer->buffer(bufferIndex));
	out += oIdx * getSampleConverter()->bytesPerSample();

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::writeToAudioOutputBufferFromPartData\n");
#endif

	if(inChannelIndex >= 0)
	{
		input = data->partDataOutConst(partNumber);
		noInputChannels = data->noOutChannels();
		iIdx = (inputSampleIndex * noInputChannels) + inChannelIndex;
		dType = data->partConst(partNumber).getDataType();
	}
	else
	{
		if(inChannelIndex == engine::e_lfeChannelIndex)
		{
			input = data->partFilterDataConst(partNumber, engine::e_lfeChannelIndex);
		}
		else
		{
			input = data->partDataCenterConst(partNumber);
		}
		iIdx = inputSampleIndex;
		noInputChannels = 1;
		dType = engine::e_SampleFloat;
	}

	getSampleConverter()->setNumberOfInputChannels(noInputChannels);
	getSampleConverter()->setNumberOfOutputChannels(noOutputChannels);
	getSampleConverter()->setVolume(m_volume);
	
	getSampleConverter()->convertAtIndex(input,iIdx,out,amount,dType);
}

//-------------------------------------------------------------------------------------------

QSharedPointer<AOQueryDevice::Device> AOLinuxALSA::copyDeviceInformation(const AOQueryDevice::Device& iDevice)
{
    QSharedPointer<AOQueryDevice::Device> pDevice(new AOQueryALSA::DeviceALSA(dynamic_cast<const AOQueryALSA::DeviceALSA&>(iDevice)));
	return pDevice;
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::processMessages()
{
	processMessagesForStop();
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::processMessagesForStop()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::processMessagesForStop\n");
#endif

	if(getStopTimeFlag())
	{
		if(getFlagStart())
		{
			common::TimeStamp dT,cT;
			
			cT = getReferenceClockTime();
			dT = getStopTimeClock();
			if(dT > cT)
			{
				tint delay;
				
				dT -= cT;
                delay = static_cast<tint>(::round(static_cast<tfloat64>(dT) * 1000.0));
				if(delay>0)
				{
					processMessagesForStopSetTimer(delay);
				}
				else
				{
					stopCodec();
				}
			}
			else
			{
				stopCodec();
			}
		}
		setStopTimeFlag(false);
	}
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::processMessagesForStopSetTimer(tint delay)
{
	QTimer::singleShot(delay,this,SLOT(onStopProcess()));
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::onStopProcess()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOLinuxALSA::onStopProcess\n");
#endif
	stopCodec();
}

//-------------------------------------------------------------------------------------------

bool AOLinuxALSA::getFlagInit() const
{
	return m_flagInit;
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::setFlagInit(bool v)
{
	m_flagInit = v;
}

//-------------------------------------------------------------------------------------------

bool AOLinuxALSA::getFlagStart() const
{
	return m_flagStart;
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::setFlagStart(bool v)
{
	m_flagStart = v;
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::allocALSAPlaybackBuffers(tint formatType, tint noChannels)
{
	tint i;
	AudioHardwareBufferALSA aBuf(m_formatTypeALSA, m_noSamplesInBufferALSA, noChannels, 0, 0);
	tint sampleSize = aBuf.sampleSize(0);
	tint bytesPerBuffer = m_noSamplesInBufferALSA * noChannels * sampleSize;
	tint totalNumberOfBuffers = (m_codec->frequency() / m_noSamplesInBufferALSA) + 1;
	
	if(totalNumberOfBuffers < 6)
	{
		totalNumberOfBuffers = 6;
	}
	common::Log::g_Log.print("AOLinuxALSA::allocALSAPlaybackBuffers total=%d, sampleSize=%d, size=%d, freq=%d, chs=%d\n",
							 totalNumberOfBuffers, sampleSize, bytesPerBuffer, m_codec->frequency(), noChannels);

	freeALSAPlaybackBuffers();
	for(i = 0; i < totalNumberOfBuffers; i++)
	{
		tbyte *buffer = new tbyte [bytesPerBuffer];
		m_playbackALSAMemoryBuffers.enqueue(buffer);
	}
	m_playbackALSAMemoryBufferSize = bytesPerBuffer;
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::freeALSAPlaybackBuffers()
{
	common::Log::g_Log.print("AOLinuxALSA::freeALSAPlaybackBuffers\n");
	while(!m_playbackALSAMemoryBuffers.isEmpty())
	{
		tbyte *buffer = m_playbackALSAMemoryBuffers.dequeue();
		delete [] buffer;
	}
	m_playbackALSAMemoryBufferSize = 0;
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSA::setCodecSampleFormatType(engine::Codec *codec, engine::RData *item)
{
	if(!item->isMixing() && m_pSampleConverter != NULL && !m_pSampleConverter->isFloat())
	{
		if(codec->dataTypesSupported() & engine::e_SampleInt32)
		{
			codec->setDataTypeFormat(engine::e_SampleInt32);
		}
		else if(codec->dataTypesSupported() & engine::e_SampleInt24)
		{
			codec->setDataTypeFormat(engine::e_SampleInt24);
		}
		else if(codec->dataTypesSupported() & engine::e_SampleInt16)
		{
			codec->setDataTypeFormat(engine::e_SampleInt16);
		}
		else
		{
			codec->setDataTypeFormat(engine::e_SampleFloat);
		}		
	}
	else
	{
		codec->setDataTypeFormat(engine::e_SampleFloat);
	}
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
