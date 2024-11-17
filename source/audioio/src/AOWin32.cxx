#include "audioio/inc/AOWin32.h"
#include "audioio/inc/AudioHardwareBufferASIO.h"
#include "audioio/inc/AudioHardwareBufferWasAPI.h"
#include "audioio/inc/AOQueryWasAPI.h"

#include <Avrt.h>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(AOFactory,AOBase, \
                            AOWin32Factory,AOWin32, \
                            "win32",false)

//-------------------------------------------------------------------------------------------

AOWin32 *AOWin32::m_instance = 0;

//-------------------------------------------------------------------------------------------

AOWin32::AOWin32(QObject *parent) : AOBase(parent),
	m_deviceType(AOQueryDevice::Device::e_deviceUnknown),
	m_driver(0),
	m_bufferSizeMin(0),
	m_bufferSizePref(0),
	m_bufferSizeMax(0),
	m_driverRunFlag(false),
	m_driverPostOutput(false),
	m_resetAudioFlag(false),
	m_latencyChangeFlag(false),
	m_pWASDevice(),
	m_pAudioClient(),
	m_pAudioRenderClient(),
	m_pAudioClock(),
	m_pWASFormat(0),
	m_bufferWASFrameCount(0),
	m_hWASThread(0),
	m_hWASEvent(0),
	m_hWASFeedbackEvent(0),
	m_wasMutex(),
	m_pSampleConverter(),
	m_wasRunThread(false),
	m_wasRunFlag(false),
	m_wasPlayExclusive(false),
	m_isVolumeDevice(false)
{
	::memset(&m_driverInfo,0,sizeof(ASIODriverInfo));
	::memset(m_bufferInfos,0,sizeof(ASIOBufferInfo) * c_kMaxOutputChannels);
	::memset(m_channelInfos,0,sizeof(ASIOChannelInfo) * c_kMaxOutputChannels);
	::memset(&m_driverCallbacks,0,sizeof(ASIOCallbacks));
	m_instance = this;
}

//-------------------------------------------------------------------------------------------

AOWin32::~AOWin32()
{
	m_instance = 0;
}

//-------------------------------------------------------------------------------------------

void AOWin32::printError(const tchar *strR,const tchar *strE) const
{
	common::Log::g_Log << "AOWin32::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool AOWin32::startAudioService()
{
	if(!m_audioStartCount)
	{
		if(!ASIODriverService::start())
		{
			common::Log::g_Log << "AOWin32::startAudioService - No ASIO service or devices." << common::c_endl;
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------

void AOWin32::stopAudioService()
{
	if(!m_audioStartCount)
	{
		ASIODriverService::stop();
	}
}

//-------------------------------------------------------------------------------------------

engine::AData *AOWin32::allocateData(tint len,tint inChannel,tint outChannel)
{
	ASIOData *d = new ASIOData(len,inChannel,outChannel);
	return reinterpret_cast<engine::AData *>(d);
}

//-------------------------------------------------------------------------------------------

bool AOWin32::isASIODevice()
{
	bool res = false;
	QSharedPointer<AOQueryDevice::Device> pDevice = getCurrentDevice();
	
	if(!pDevice.isNull())
	{
		if(pDevice->type()==AOQueryDevice::Device::e_deviceASIO)
		{
			res = true;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOWin32::isWasAPIDevice()
{
	bool res = false;
	QSharedPointer<AOQueryDevice::Device> pDevice = getCurrentDevice();
	
	if(!pDevice.isNull())
	{
		if(pDevice->type()==AOQueryDevice::Device::e_deviceWasAPI)
		{
			res = true;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOWin32::openAudio()
{
	bool res = false;
	
	closeAudio();
	
	m_frequency = m_codec->frequency();
	
	if(isASIODevice())
	{
		res = openAudioASIO();
		if(res)
		{
			m_deviceType = AOQueryDevice::Device::e_deviceASIO;
		}
	}
	else if(isWasAPIDevice())
	{
		res = openAudioWasAPI();
		if(res)
		{
			m_deviceType = AOQueryDevice::Device::e_deviceWasAPI;
		}
	}
	else
	{
		printError("openAudio","Unknown audio device type");
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOWin32::openAudioASIO()
{
	tint i;
	bool res = false;
	
	m_driverCallbacks.bufferSwitch = AOWin32::bufferSwitch;
	m_driverCallbacks.sampleRateDidChange = AOWin32::sampleRateChanged;
	m_driverCallbacks.asioMessage = AOWin32::asioMessages;
	m_driverCallbacks.bufferSwitchTimeInfo = AOWin32::bufferSwitchTimeInfo;
	
	::memset(m_bufferInfos,0,sizeof(ASIOBufferInfo) * c_kMaxOutputChannels);
	
	if(ASIODriverService::instance().open(m_deviceIdx))
	{
		m_driver = ASIODriverService::instance().driverPtr(m_deviceIdx);
		if(m_driver!=0)
		{
			if(m_driver->ASIOInit(&m_driverInfo)==ASE_OK)
			{
				long numInputChs = 0,numOutputChs = 0;
				
				if(m_driver->ASIOGetChannels(&numInputChs,&numOutputChs)==ASE_OK)
				{
					m_noChannels = numOutputChs;
					
					for(i=0;i<m_noChannels;++i)
					{
						m_bufferInfos[i].isInput = ASIOFalse;
						m_bufferInfos[i].channelNum = i;
					}
					
					if(createAudioBuffers())
					{
						long inputLatency,outputLatency;
							
						if(m_driver->ASIOGetLatencies(&inputLatency,&outputLatency)==ASE_OK)
						{
							m_outputLatencyTime = static_cast<tfloat64>(outputLatency) / static_cast<tfloat64>(m_frequency);
							res = true;
								
							::memset(m_channelInfos,0,sizeof(ASIOChannelInfo) * c_kMaxOutputChannels);
							for(i=0;i<m_noChannels;++i)
							{
								m_channelInfos[i].channel = m_bufferInfos[i].channelNum;
								m_channelInfos[i].isInput = m_bufferInfos[i].isInput;
								
								if(m_driver->ASIOGetChannelInfo(&m_channelInfos[i])==ASE_OK)
								{
									tint sSize = ASIODriverService::getSampleSize(m_channelInfos[i].type);
									::memset(m_bufferInfos[i].buffers[0],0,sSize * m_bufferSizePref);
									::memset(m_bufferInfos[i].buffers[1],0,sSize * m_bufferSizePref);
								}
								else
								{
									printError("openAudio","Error getting channel information");
									res = false;
								}
							}

							if(openAudioFrequency())
							{
								initCyclicBuffer();
								{
									AudioItem *item = m_callbackAudioItem;
									
									for(i=0;i<m_noOfCyclicBufferItems;++i)
									{
										ASIOData *aData = dynamic_cast<ASIOData *>(item->data());
										aData->setSampleType(m_channelInfos[0].type);
										item = item->prev();
									}
								}
									
								m_driverPostOutput = (m_driver->ASIOOutputReady()==ASE_OK) ? true : false;
							}
							else
							{
								QString err("Failed to set audio ASIO driver '");
								err += ASIODriverService::instance().driverInfo(m_deviceIdx).name();
								err += "' to sample rate " + QString::number(m_frequency) + "Hz";
								printError("openAudio",err.toUtf8().constData());							
							}
						}
						else
						{
							printError("openAudio","Error getting device audio latencies");
						}
					}
					else
					{
						printError("openAudio","Error creating ASIO audio buffers");
					}
				}
				else
				{
					printError("openAudio","Error in getting number of channels");
				}
			}
			else
			{
				QString err;
				err = "Failed to initialize audio ASIO driver '" + ASIODriverService::instance().driverInfo(m_deviceIdx).name() + "'";
				printError("openAudio",err.toUtf8().constData());				
			}
		}
		else
		{
			printError("openAudio","Failed to get expected device driver");
		}
	}
	else
	{
		QString err;
		err = "Failed to open audio ASIO driver '" + ASIODriverService::instance().driverInfo(m_deviceIdx).name() + "'";
		printError("openAudio",err.toUtf8().constData());
	}
	
	if(!res)
	{
		closeAudio();
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOWin32::createAudioBuffers()
{
	bool res = false;
	long min,pref,max,gran;
	
	if(getASIODriver()!=0)
	{
		if(getASIODriver()->ASIOGetBufferSize(&min,&max,&pref,&gran)==ASE_OK)
		{
			m_bufferSizeMin = static_cast<tint>(min);
			m_bufferSizeMax = static_cast<tint>(max);

			if(m_bufferSizeMin > m_bufferSizeMax)
			{
				tint t = m_bufferSizeMin;
				m_bufferSizeMin = m_bufferSizeMax;
				m_bufferSizeMax = t;
			}

			m_bufferSizePref = static_cast<tint>(pref);	
			
			if(getASIODriver()->ASIOCreateBuffers(m_bufferInfos,m_noChannels,m_bufferSizePref,&m_driverCallbacks)==ASE_OK)
			{
				res = true;
			}
			else
			{
				bool retry = false;
			
				if(m_bufferSizePref < m_bufferSizeMin)
				{
					m_bufferSizePref = m_bufferSizeMin;
					retry = true;
				}
				else if(m_bufferSizePref > m_bufferSizeMax)
				{
					m_bufferSizePref = m_bufferSizeMax;
					retry = true;
				}
				
				if(retry)
				{
					if(getASIODriver()->ASIOCreateBuffers(m_bufferInfos,m_noChannels,m_bufferSizePref,&m_driverCallbacks)==ASE_OK)
					{
						res = true;
					}
				}
			}
			
			if(!res)
			{
				printError("createAudioBuffers","Failed to create ASIO audio device buffers");
			}
		}
		else
		{
			printError("createAudioBuffers","Error getting buffer size for ASIO device");
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

ASIODriver *AOWin32::getASIODriver()
{
	return m_driver;
}

//-------------------------------------------------------------------------------------------

bool AOWin32::openAudioFrequency()
{
	int iFreq = m_frequency;
	ASIOSampleRate currentRate;
	
	m_deviceInfoMutex.lock();
	const AOQueryDevice::Device& dev = m_deviceInfo->device(m_deviceIdx);
	const QSet<int>& sFreqSet = dev.frequencies();
	QSet<int>::const_iterator ppI;

	if(dev.isFrequencySupported(m_frequency))
	{
		if(m_driver->ASIOSetSampleRate(static_cast<ASIOSampleRate>(m_frequency))==ASE_OK)
		{
			m_deviceInfoMutex.unlock();
			return true;
		}
	}
	
	int cPFreq = 0;
	for(ppI=sFreqSet.begin();ppI!=sFreqSet.end();++ppI)
	{
		int sFreq = *ppI;
		if(m_frequency < sFreq)
		{
			int diff = sFreq - m_frequency;
			if(cPFreq>0)
			{
				if(diff < (cPFreq - m_frequency))
				{
					cPFreq = sFreq;
				}
			}
			else
			{
				cPFreq = sFreq;
			}
		}
	}
	if(cPFreq>0)
	{
		if(m_driver->ASIOSetSampleRate(static_cast<ASIOSampleRate>(cPFreq))==ASE_OK)
		{
			m_frequency = cPFreq;
			initResampler(iFreq,m_frequency);
			m_deviceInfoMutex.unlock();
			return true;
		}
	}
	
	for(ppI=sFreqSet.begin();ppI!=sFreqSet.end();++ppI)
	{
		int sFreq = *ppI;
		if(m_frequency > sFreq)
		{
			int diff = m_frequency - sFreq;
			if(cPFreq>0)
			{
				if(diff < (m_frequency - cPFreq))
				{
					cPFreq = sFreq;
				}
			}
			else
			{
				cPFreq = sFreq;
			}
		}
	}
	if(cPFreq>0)
	{
		if(m_driver->ASIOSetSampleRate(static_cast<ASIOSampleRate>(cPFreq))==ASE_OK)
		{
			m_frequency = cPFreq;
			initResampler(iFreq,m_frequency);
			m_deviceInfoMutex.unlock();
			return true;
		}
	}
	
	if(m_driver->ASIOGetSampleRate(&currentRate)==ASE_OK)
	{
		m_frequency = static_cast<tint>(currentRate);
		initResampler(iFreq,m_frequency);
		m_deviceInfoMutex.unlock();
		return true;
	}
	
	m_deviceInfoMutex.unlock();
	return false;
}

//-------------------------------------------------------------------------------------------

void AOWin32::closeAudio()
{
	if(m_deviceType==AOQueryDevice::Device::e_deviceASIO)
	{
		closeAudioASIO();
	}
	else if(m_deviceType==AOQueryDevice::Device::e_deviceWasAPI)
	{
		closeAudioWasAPI();
	}
	m_deviceType = AOQueryDevice::Device::e_deviceUnknown;
	closeResampler();
}

//-------------------------------------------------------------------------------------------

void AOWin32::closeAudioASIO()
{
	if(m_driver!=0)
	{
		stopAudioDevice();
		
		if(m_bufferInfos[0].buffers[0]!=0)
		{
			if(m_driver->ASIODisposeBuffers()!=ASE_OK)
			{
				printError("closeAudio","Error releasing ASIO audio buffers");
			}
		}
		if(m_driver->ASIOExit()!=ASE_OK)
		{
			printError("closeAudio","Error exiting ASIO device");
		}
		ASIODriverService::instance().close(m_deviceIdx);
		::memset(&m_driverInfo,0,sizeof(ASIODriverInfo));
		::memset(m_bufferInfos,0,sizeof(ASIOBufferInfo) * c_kMaxOutputChannels);
		::memset(m_channelInfos,0,sizeof(ASIOChannelInfo) * c_kMaxOutputChannels);
		::memset(&m_driverCallbacks,0,sizeof(ASIOCallbacks));
		m_driver = 0;
	}	
}

//-------------------------------------------------------------------------------------------

bool AOWin32::startAudioDevice()
{
	bool res = false;

	if(m_deviceType==AOQueryDevice::Device::e_deviceASIO)
	{
		res = startAudioDeviceASIO();
	}
	else if(m_deviceType==AOQueryDevice::Device::e_deviceWasAPI)
	{
		res = startAudioDeviceWasAPI();
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOWin32::startAudioDeviceASIO()
{
	bool res = false;
	
	if(m_driver!=0)
	{
		if(m_driver->ASIOStart()==ASE_OK)
		{
			m_driverRunFlag = true;
			res = true;
		}
		else
		{
			printError("startAudioDevice","Failed to start audio device playback");
		}
	}
	else
	{
		printError("startAudioDevice","No driver open to start");
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void AOWin32::stopAudioDevice()
{
	if(m_deviceType==AOQueryDevice::Device::e_deviceASIO)
	{
		stopAudioDeviceASIO();
	}
	else if(m_deviceType==AOQueryDevice::Device::e_deviceWasAPI)
	{
		stopAudioDeviceWasAPI();
	}
}

//-------------------------------------------------------------------------------------------

void AOWin32::stopAudioDeviceASIO()
{
	if(m_driverRunFlag)
	{
		if(m_driver->ASIOStop()!=ASE_OK)
		{
			printError("stopAudioDevice","Error stopping ASIO audio stream");
		}
		m_driverRunFlag = false;
	}
}

//-------------------------------------------------------------------------------------------

bool AOWin32::isAudio() const
{
	bool res = false;

	if(m_deviceType==AOQueryDevice::Device::e_deviceASIO)
	{
		res = isAudioASIO();
	}
	else if(m_deviceType==AOQueryDevice::Device::e_deviceWasAPI)
	{
		res = isAudioWasAPI();
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOWin32::isAudioASIO() const
{
	return (m_driver!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

void AOWin32::sampleRateChanged(ASIOSampleRate sRate)
{
	m_instance->sampleRateChangedI(sRate);
}

//-------------------------------------------------------------------------------------------

void AOWin32::sampleRateChangedI(ASIOSampleRate)
{}

//-------------------------------------------------------------------------------------------

long AOWin32::asioMessages(long selector,long value,void *message,double *opt)
{
	return m_instance->asioMessagesI(selector,value,message,opt);
}

//-------------------------------------------------------------------------------------------

long AOWin32::asioMessagesI(long selector,long value,void *,double *)
{
	long ret = 0;
	
#if defined(DEBUG_LOG_AUDIOOUTPUT)
	common::Log::g_Log.print("asioMessageI - %d\n",selector);
#endif

	switch(selector)
	{
		case kAsioSelectorSupported:
			if(value==kAsioResetRequest || value==kAsioEngineVersion || value==kAsioResyncRequest ||
			   value==kAsioLatenciesChanged || value==kAsioSupportsTimeInfo || value==kAsioSupportsTimeCode ||
			   value==kAsioSupportsInputMonitor)
			{
				ret = 1L;
			}
			break;
			
		case kAsioResetRequest:
			m_resetAudioFlag = true;
			ret = 1L;
			break;

		case kAsioBufferSizeChange:
			ret = 1L;
			break;
			
		case kAsioResyncRequest:
			ret = 1L;
			break;
			
		case kAsioLatenciesChanged:
			m_latencyChangeFlag = true;
			ret = 1L;
			break;
			
		case kAsioEngineVersion:
			ret = 2L;
			break;
			
		case kAsioSupportsTimeInfo:
			ret = 1L;
			break;
			
		case kAsioSupportsTimeCode:
			ret = 0L;
			break;
	}
	return ret;
}

//-------------------------------------------------------------------------------------------

void AOWin32::processMessages()
{
	if(m_deviceType==AOQueryDevice::Device::e_deviceASIO)
	{
		processMessagesASIO();
	}
	else if(m_deviceType==AOQueryDevice::Device::e_deviceWasAPI)
	{
		processMessagesWasAPI();
	}
	processStopTimeMessage();
}

//-------------------------------------------------------------------------------------------

void AOWin32::processStopTimeMessage()
{
	if(m_stopTimeFlag)
	{
		common::TimeStamp dT,rT;
		
		dT = const_cast<const common::TimeStamp &>(m_stopTimeClock);
		rT = common::TimeStamp::reference();
		if(dT > rT)
		{
			tint delay;
			
			dT -= rT;
			delay = (dT.secondsTotal() * 1000) + dT.millisecond();
			QTimer::singleShot(delay,this,SLOT(onStop()));
		}
		else
		{
			stopCodec();
		}
		m_stopTimeFlag = false;
	}
}

//-------------------------------------------------------------------------------------------

void AOWin32::processMessagesASIO()
{
	if(m_resetAudioFlag)
	{
		if(!resetPlayback())
		{
			printError("processMessages","Error resetting audio device");
		}
		m_resetAudioFlag = false;
	}
	
	if(m_latencyChangeFlag)
	{
		long inputLatency = 0,outputLatency = 0;
		common::TimeStamp nLatency;
		
		if(m_driver!=0)
		{
			if(m_driver->ASIOGetLatencies(&inputLatency,&outputLatency)==ASE_OK)
			{
				common::TimeStamp lT;
				
				nLatency = static_cast<tfloat64>(outputLatency) / static_cast<tfloat64>(m_frequency);
				if(nLatency >= m_outputLatencyTime)
				{
					// Latency now longer so 
					lT = nLatency - m_outputLatencyTime;
					m_audioStartClock += lT;
					m_currentOutTime += lT;
				}
				else
				{
					lT = m_outputLatencyTime - nLatency;
					m_audioStartClock -= lT;
					m_currentOutTime -= lT;
				}
				m_outputLatencyTime = nLatency;
			}
			else
			{
				printError("processMessages","Error getting new audio latency information");
			}
		}
		m_latencyChangeFlag = false;
	}
}

//-------------------------------------------------------------------------------------------

void AOWin32::onStop()
{
	stopCodec();
}

//-------------------------------------------------------------------------------------------

void AOWin32::bufferSwitch(long index,ASIOBool processNow)
{
	m_instance->bufferSwitchI(index,processNow);
}

//-------------------------------------------------------------------------------------------

void AOWin32::bufferSwitchI(long index,ASIOBool processNow)
{
	if(m_deviceType==AOQueryDevice::Device::e_deviceASIO)
	{
		ASIOTime timeInfo;
	
		::memset(&timeInfo,0,sizeof(ASIOTime));
		if(m_driver->ASIOGetSamplePosition(&timeInfo.timeInfo.samplePosition,&timeInfo.timeInfo.systemTime)==ASE_OK)
		{
			timeInfo.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;
		}
		bufferSwitchTimeInfoI(&timeInfo,index,processNow);
	}
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOWin32::getTimeFromASIO(const ASIOTimeStamp *tS) const
{
	common::TimeStamp x;

#if NATIVE_INT64
	x.nano64(static_cast<tuint64>(*tS));
#else
	x.nano64(tS->hi,tS->lo);
#endif
	return x;
}

//-------------------------------------------------------------------------------------------

void AOWin32::getTimeFromOmega(const common::TimeStamp& t,ASIOTimeStamp *tS) const
{
	tuint64 x = t.nano64();
#if NATIVE_INT64
	*tS = static_cast<ASIOTimeStamp>(x);
#else
	tS->hi = static_cast<unsigned long>((x >> 32) & 0x00000000ffffffffULL);
	tS->lo = static_cast<unsigned long>((x      ) & 0x00000000ffffffffULL);
#endif
}

//-------------------------------------------------------------------------------------------
// IOProc refactor
//-------------------------------------------------------------------------------------------

ASIOTime *AOWin32::bufferSwitchTimeInfo(ASIOTime *timeInfo,long index,ASIOBool processNow)
{
	return m_instance->bufferSwitchTimeInfoI(timeInfo,index,processNow);
}

//-------------------------------------------------------------------------------------------

ASIOTime *AOWin32::bufferSwitchTimeInfoI(ASIOTime *timeInfo,long index,ASIOBool)
{
	IOTimeStamp systemTime;
	AudioHardwareBufferASIO buffer(m_bufferInfos,m_channelInfos,m_bufferSizePref,m_noChannels,index);
	systemTime = createIOTimeStamp(timeInfo);
	writeToAudioIOCallback(&buffer,systemTime);
	return 0L;
}

//-------------------------------------------------------------------------------------------

IOTimeStamp AOWin32::createIOTimeStamp(const ASIOTime *sysTime) const
{
	common::TimeStamp tS;
	bool valid = (sysTime->timeInfo.flags & kSystemTimeValid) ? true : false;
	if(valid)
	{
		tS = getTimeFromASIO(&(sysTime->timeInfo.systemTime));
	}
	return IOTimeStamp(valid,tS);
}

//-------------------------------------------------------------------------------------------

void AOWin32::writeToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,
                                                  const engine::RData *data,
                                                  tint partNumber,
                                                  tint inChannelIndex,
                                                  tint bufferIndex,
                                                  tint outChannelIndex,
                                                  tint inputSampleIndex,
                                                  tint outputSampleIndex,
                                                  tint amount)
{
	if(m_deviceType==AOQueryDevice::Device::e_deviceASIO)
	{
		writeToAudioOutputBufferFromPartDataASIO(pBuffer,data,partNumber,inChannelIndex,bufferIndex,outChannelIndex,inputSampleIndex,outputSampleIndex,amount);
	}
	else if(m_deviceType==AOQueryDevice::Device::e_deviceWasAPI)
	{
		writeToAudioOutputBufferFromPartDataWasAPI(pBuffer,data,partNumber,inChannelIndex,bufferIndex,outChannelIndex,inputSampleIndex,outputSampleIndex,amount);
	}
}

//-------------------------------------------------------------------------------------------

void AOWin32::writeToAudioOutputBufferFromPartDataASIO(AbstractAudioHardwareBuffer *pBuffer,
                                                       const engine::RData *data,
                                                       tint partNumber,
                                                       tint inChannelIndex,
                                                       tint bufferIndex,
                                                       tint ,
                                                       tint inputSampleIndex,
                                                       tint outputSampleIndex,
                                                       tint amount)
{
	const ASIOData *aData = reinterpret_cast<const ASIOData *>(data);
	const tbyte *input = reinterpret_cast<const tbyte *>(aData->asioDataConst(inChannelIndex,partNumber));
	tint iIdx = inputSampleIndex * aData->getSampleSize();
	
	tbyte *out = pBuffer->buffer(bufferIndex);
	tint oIdx = outputSampleIndex * pBuffer->numberOfChannelsInBuffer(bufferIndex) * pBuffer->sampleSize(bufferIndex);
	
	if(input!=0)
	{
		memcpy(&out[oIdx],&input[iIdx],amount * pBuffer->sampleSize(bufferIndex));
	}
	else
	{
		memset(&out[oIdx],0,amount * pBuffer->sampleSize(bufferIndex));
	}
}

//-------------------------------------------------------------------------------------------

void AOWin32::processDataForOutput(engine::RData *data)
{
	if(m_deviceType==AOQueryDevice::Device::e_deviceASIO)
	{
		ASIOData *asioData = dynamic_cast<ASIOData *>(data);
		asioData->setVolume(m_volume);
		asioData->convert();
	}
}

//-------------------------------------------------------------------------------------------

void AOWin32::writeToAudioPostProcess()
{
	if(m_deviceType==AOQueryDevice::Device::e_deviceASIO)
	{
		if(m_driverPostOutput)
		{
			m_driver->ASIOOutputReady();
		}
	}
}

//-------------------------------------------------------------------------------------------

QSharedPointer<AOQueryDevice::Device> AOWin32::copyDeviceInformation(const AOQueryDevice::Device& iDevice)
{
	QSharedPointer<AOQueryDevice::Device> pDevice;
	
	if(iDevice.type()==AOQueryDevice::Device::e_deviceASIO)
	{
		QSharedPointer<AOQueryDevice::Device> nDevice(new AOQueryASIO::DeviceASIO(iDevice));
		pDevice = nDevice;
	}
	else if(iDevice.type()==AOQueryDevice::Device::e_deviceWasAPI)
	{
		QSharedPointer<AOQueryDevice::Device> nDevice(new AOQueryWasAPI::DeviceWasAPI(dynamic_cast<const AOQueryWasAPI::DeviceWasAPI&>(iDevice)));
		pDevice = nDevice;
	}
	return pDevice;
}

//-------------------------------------------------------------------------------------------
// WasAPI
//-------------------------------------------------------------------------------------------

bool AOWin32::openAudioWasAPI()
{
	bool res = false;
	
	if(openAudioWasAPIImpl())
	{
		res = true;
	}
	else
	{
		QString errStr = QString("Failed to open audio in %1 mode").arg(isExclusive() ? "exclusive" : "shared" );
		printError("openAudioWasAPI",errStr.toUtf8().constData());
		closeAudioWasAPI();
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOWin32::openAudioWasAPIImpl()
{
	bool res = false;
	QSharedPointer<AOQueryWasAPI::DeviceWasAPI> pDevice = getCurrentDevice().dynamicCast<AOQueryWasAPI::DeviceWasAPI>();

	if(!pDevice.isNull() && pDevice->type()==AOQueryDevice::Device::e_deviceWasAPI)
	{
		m_pWASDevice = pDevice->deviceInterface();
			
		if(!m_pWASDevice.isNull())
		{
			m_pAudioClient = m_pWASDevice->getAudioClient();
			if(!m_pAudioClient.isNull())
			{				
				m_pWASFormat = m_pWASDevice->findClosestSupportedFormat(getSourceDescription(pDevice->noChannels()));
				if(m_pWASFormat!=0)
				{
					if(getFrequency() != m_pWASFormat->nSamplesPerSec)
					{
						int iFreq = getFrequency();
						setFrequency(m_pWASFormat->nSamplesPerSec);
						initResampler(iFreq,getFrequency());
					}
					initCyclicBuffer();
						
					m_pSampleConverter = createWASSampleConverter(m_pWASFormat);
					if(!m_pSampleConverter.isNull())
					{
						if(activateWasAPIAudioDevice(m_pWASFormat))
						{
							m_hWASEvent = CreateEvent(0,FALSE,FALSE,0);
							m_hWASFeedbackEvent = CreateEvent(0,TRUE,FALSE,0);
								
							if(m_hWASEvent!=0 && m_hWASFeedbackEvent!=0)
							{
								DWORD threadID;
									
								m_wasRunThread = true;
								m_hWASThread = CreateThread(0,0,AOWin32::writeWASAudioThread,reinterpret_cast<LPVOID>(this),0,&threadID);
								if(m_hWASThread!=0)
								{
									HRESULT hr;
										
									hr = m_pAudioClient->SetEventHandle(m_hWASEvent);
									if(hr==S_OK)
									{
										hr = m_pAudioClient->GetBufferSize(&m_bufferWASFrameCount);
										if(hr==S_OK)
										{
											const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
											IAudioRenderClient *pRender = 0;
												
											hr = m_pAudioClient->GetService(IID_IAudioRenderClient,reinterpret_cast<void **>(&pRender));
											if(hr==S_OK)
											{
												IAudioRenderClientIFSPtr pNewRender(new IAudioRenderClientIF(pRender));
												m_pAudioRenderClient = pNewRender;
													
												const IID IID_IAudioClock = __uuidof(IAudioClock);
												IAudioClock *pClock = 0;
												hr = m_pAudioClient->GetService(IID_IAudioClock,reinterpret_cast<void **>(&pClock));
												if(hr==S_OK)
												{
													IAudioClockIFSPtr pNewClock(new IAudioClockIF(pClock));
													m_pAudioClock = pNewClock;
													openAudioWasAPIVolume();
													res = true;
												}
												else
												{
													printError("openAudioWasAPIWithExclusion","Failed to create audio clock");
												}
											}
											else
											{
												printError("openAudioWasAPIWithExclusion","Failed to create audio renderer");
											}
										}
										else
										{
											printError("openAudioWasAPIWithExclusion","Failed to get buffer size");
										}
									}
									else
									{
										printError("openAudioWasAPIWithExclusion","Error setting audio write event");
									}
								}
								else
								{
									printError("openAudioWasAPIWithExclusion","Failed to create audio writing thread");
								}
							}
							else
							{
								printError("openAudioWasAPIWithExclusion","Error creating event handler");
							}
						}
						else
						{
							printError("openAudioWasAPIWithExclusion","Failed to open audio device");
						}
					}
					else
					{
						printError("openAudioWasAPIWithExclusion","Failed to create sample converter audio");
					}
				}
				else
				{
					printError("openAudioWasAPIWithExclusion","Unable to find output format");
				}
			}
			else
			{
				printError("openAudioWasAPIWithExclusion","Failed to create audio client interface");
			}
		}
		else
		{
			printError("openAudioWasAPIWithExclusion","Failed to find WasAPI device");
		}
	}
	else
	{
		printError("openAudioWasAPIWithExclusion","Current device not associated with WasAPI");
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void AOWin32::closeAudioWasAPI()
{
	if(m_hWASEvent!=0)
	{
		if(m_hWASThread!=0)
		{
			m_wasMutex.lock();
			m_wasRunThread = false;
			SetEvent(m_hWASEvent);
			m_wasMutex.unlock();
			WaitForSingleObject(m_hWASThread,INFINITE);
			m_hWASThread = 0;
		}
		CloseHandle(m_hWASEvent);
		m_hWASEvent = 0;
	}	
	if(m_hWASFeedbackEvent!=0)
	{
		CloseHandle(m_hWASFeedbackEvent);
		m_hWASFeedbackEvent = 0;
	}
	
	closeAudioWasAPIVolume();
	
	if(!m_pAudioClock.isNull())
	{
		m_pAudioClock.clear();
	}
	if(!m_pAudioRenderClient.isNull())
	{
		m_pAudioRenderClient.clear();
	}
	if(!m_pAudioClient.isNull())
	{
		m_pAudioClient.clear();
	}
	if(!m_pWASDevice.isNull())
	{
		m_pWASDevice->releaseAudioClient();
		m_pWASDevice.clear();
	}
	if(m_pWASFormat!=0)
	{
		delete m_pWASFormat;
		m_pWASFormat = 0;
	}
}

//-------------------------------------------------------------------------------------------

bool AOWin32::startAudioDeviceWasAPI()
{
	bool res = false;
	
	if(!m_pAudioRenderClient.isNull() && !m_pAudioClock.isNull() && m_hWASThread!=0)
	{
		HRESULT hr;
		
		m_wasMutex.lock();
		ResetEvent(m_hWASFeedbackEvent);
		m_wasMutex.unlock();
		SetEvent(m_hWASEvent);
		WaitForSingleObject(m_hWASFeedbackEvent,INFINITE);
		
		hr = m_pAudioClient->Start();
		if(hr==S_OK)
		{
			m_wasRunFlag = true;
			res = true;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void AOWin32::stopAudioDeviceWasAPI()
{
	if(m_wasRunFlag && !m_pAudioClient.isNull())
	{
		m_pAudioClient->Stop();
	}
	m_wasRunFlag = false;
}

//-------------------------------------------------------------------------------------------

bool AOWin32::isAudioWasAPI() const
{
	return (m_hWASThread!=0);
}

//-------------------------------------------------------------------------------------------

void AOWin32::openAudioWasAPIVolume()
{
	m_isVolumeDevice = false;
	
	if(!m_pWASDevice.isNull() && m_pWASDevice->isDeviceVolume())
	{
		m_isVolumeDevice = true;
		m_pWASDevice->setupVolumeNotification(AOWin32::onVolumeChangeNotification, reinterpret_cast<LPVOID>(this));
		m_volume = m_pWASDevice->getVolume();
		emitOnVolumeChanged(m_volume);
	}
}

//-------------------------------------------------------------------------------------------

void AOWin32::closeAudioWasAPIVolume()
{
	if(m_isVolumeDevice && !m_pWASDevice.isNull())
	{
		m_pWASDevice->shutdownVolumeNotification();
	}
	m_isVolumeDevice = false;
}

//-------------------------------------------------------------------------------------------

void AOWin32::onVolumeChangeNotification(LPVOID pVInstance, sample_t vol)
{
	if(pVInstance != 0)
	{
		AOWin32 *pInstance = reinterpret_cast<AOWin32 *>(pVInstance);
		pInstance->onVolumeChangeNotification(vol);
	}
}

//-------------------------------------------------------------------------------------------

void AOWin32::onVolumeChangeNotification(sample_t vol)
{
	AudioEvent *e = new AudioEvent(AudioEvent::e_setVolumeEvent);
	
	if(vol < 0.0)
	{
		vol = 0.0;
	}
	else if(vol > 1.0)
	{
		vol = 1.0;
	}
	e->volume() = vol;
	e->isCallback() = true;
	QCoreApplication::postEvent(this,e);
}

//-------------------------------------------------------------------------------------------

void AOWin32::doSetVolume(sample_t vol, bool isCallback)
{
	AOBase::doSetVolume(vol, isCallback);
	if(isWasAPIDevice() && m_isVolumeDevice && !m_pWASDevice.isNull() && !isCallback)
	{
		if(!m_pWASDevice->setVolume(vol))
		{
			m_volume = m_pWASDevice->getVolume();
			emitOnVolumeChanged(m_volume);
		}
	}
}

//-------------------------------------------------------------------------------------------

void AOWin32::writeToAudioOutputBufferFromPartDataWasAPI(AbstractAudioHardwareBuffer *pBuffer,
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
	out += oIdx * m_pSampleConverter->bytesPerSample();
	
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
	m_pSampleConverter->setNumberOfInputChannels(noInputChannels);
	m_pSampleConverter->setNumberOfOutputChannels(noOutputChannels);
	m_pSampleConverter->setVolume(m_volume);

	if(m_isVolumeDevice)
	{
		m_pSampleConverter->setVolume(c_plusOneSample);
	}
	else
	{
		m_pSampleConverter->setVolume(m_volume);
	}
	
	m_pSampleConverter->convertAtIndex(input,iIdx,out,amount,dType);
}

//-------------------------------------------------------------------------------------------

void AOWin32::processMessagesWasAPI()
{}

//-------------------------------------------------------------------------------------------

REFERENCE_TIME AOWin32::alignedBufferDuration(WAVEFORMATEX* pFormat)
{
	HRESULT hr;
	REFERENCE_TIME duration = 10000000;
	UINT32 nFrames = 0;

	hr = m_pAudioClient->GetBufferSize(&nFrames);
	if(hr == S_OK)
	{
		duration = static_cast<REFERENCE_TIME>((10000.0 * 1000 / pFormat->nSamplesPerSec * nFrames) + 0.5);
	}
	else
	{
		printError("alignedBufferDuration", "Failed to get buffer size");
	}
	return duration;
}

//-------------------------------------------------------------------------------------------

bool AOWin32::activateWasAPIAudioDevice(WAVEFORMATEX *pFormat)
{
	HRESULT hr;
	AUDCLNT_SHAREMODE shareMode;
	REFERENCE_TIME bufferDurationL = 0, bufferDurationH;
	bool res = false;

	if(m_pWASDevice->isExclusive())
	{
		shareMode = AUDCLNT_SHAREMODE_EXCLUSIVE;
		if(m_pAudioClient->GetDevicePeriod(&bufferDurationH,0)!=S_OK)
		{
			bufferDurationH = 10000000;
		}
		bufferDurationL = bufferDurationH;
		m_wasPlayExclusive = true;
	}
	else
	{
		shareMode = AUDCLNT_SHAREMODE_SHARED;
		bufferDurationH = 10000000;
		m_wasPlayExclusive = false;
	}
	
	hr = m_pAudioClient->Initialize(shareMode,AUDCLNT_STREAMFLAGS_EVENTCALLBACK, bufferDurationL, bufferDurationH,pFormat,0);
	if(hr==S_OK)
	{
		res = true;
	}
	else if(hr == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED && m_pWASDevice->isExclusive())
	{
		bufferDurationH = alignedBufferDuration(pFormat);
		hr = m_pAudioClient->Initialize(shareMode, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, bufferDurationH, bufferDurationH, pFormat, 0);
		if(hr == S_OK)
		{
			res = true;
		}
		else
		{
			printError("activateWasAPIAudioDevice", "Failed to initialize audio device after realigning buffer");
		}
	}
	else
	{
		printError("activateWasAPIAudioDevice","Failed to initialize audio device");
	}
	return res;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<SampleConverter> AOWin32::createWASSampleConverter(WAVEFORMATEX *pFormat)
{
	QSharedPointer<SampleConverter> pConverter;

	if(pFormat->cbSize>=22 && pFormat->wFormatTag==WAVE_FORMAT_EXTENSIBLE)
	{
		WAVEFORMATEXTENSIBLE *pFormatEx = reinterpret_cast<WAVEFORMATEXTENSIBLE *>(pFormat);
		
		if(pFormatEx->SubFormat==KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
		{
			bool isSinglePrecision = (pFormatEx->Format.wBitsPerSample==64) ? false : true;
			QSharedPointer<SampleConverter> pNC(new SampleConverter(isSinglePrecision,true));
			pConverter = pNC;
		}
		else
		{
			QSharedPointer<SampleConverter> pNC(new SampleConverter(pFormatEx->Samples.wValidBitsPerSample,pFormatEx->Format.wBitsPerSample/8,true,true,true));
			pConverter = pNC;
		}
	}
	else
	{
		if(pFormat->wFormatTag==WAVE_FORMAT_IEEE_FLOAT)
		{
			bool isSinglePrecision = (pFormat->wBitsPerSample==64) ? false : true;
			QSharedPointer<SampleConverter> pNC(new SampleConverter(isSinglePrecision,true));
			pConverter = pNC;		
		}
		else
		{
			QSharedPointer<SampleConverter> pNC(new SampleConverter(pFormat->wBitsPerSample,pFormat->wBitsPerSample/8,true,true,true));
			pConverter = pNC;
		}
	}
	return pConverter;
}

//-------------------------------------------------------------------------------------------

IOTimeStamp AOWin32::createIOTimeStampWasAPI() const
{
	HRESULT hr;
	UINT64 position = 0,freq = 0;
	common::TimeStamp tS;
	bool valid = false;

	hr = m_pAudioClock->GetPosition(&position,0);
	if(hr==S_OK)
	{
		hr = m_pAudioClock->GetFrequency(&freq);
		if(hr==S_OK)
		{
			tS = static_cast<tfloat64>(position) / static_cast<tfloat64>(freq);
			valid = true;
		}
	}
	return IOTimeStamp(valid,tS);
}

//-------------------------------------------------------------------------------------------

DWORD WINAPI AOWin32::writeWASAudioThread(LPVOID arg)
{
	AOWin32 *pAudio = reinterpret_cast<AOWin32 *>(arg);
	pAudio->writeWASAudioThreadImpl();
	return 0;
}

//-------------------------------------------------------------------------------------------

void AOWin32::writeWASAudioThreadImpl()
{
	HANDLE hTask;
	DWORD taskIndex = 0;
	bool loop = true;

	hTask = AvSetMmThreadCharacteristics(TEXT("Pro Audio"),&taskIndex);

	while(loop)
	{
		WaitForSingleObject(m_hWASEvent,INFINITE);
		
		m_wasMutex.lock();
		if(m_wasRunThread)
		{
			writeWASAudio();
		}
		else
		{
			loop = false;
		}
		SetEvent(m_hWASFeedbackEvent);
		m_wasMutex.unlock();
	}
	
    if (hTask != NULL)
    {
        AvRevertMmThreadCharacteristics(hTask);
    }
}

//-------------------------------------------------------------------------------------------

void AOWin32::writeWASAudio()
{
	UINT32 numFramesAvailable, numFramesPadding = 0;
	BYTE *pData = 0;
	HRESULT hr = S_OK;
	
	if(!m_wasPlayExclusive)
	{
		hr = m_pAudioClient->GetCurrentPadding(&numFramesPadding);
	}
	if(hr == S_OK)
	{
		numFramesAvailable = m_bufferWASFrameCount - numFramesPadding;
		hr = m_pAudioRenderClient->GetBuffer(numFramesAvailable, &pData);
		if(hr == S_OK)
		{
			IOTimeStamp systemTime = createIOTimeStampWasAPI();
			AudioHardwareBufferWasAPI buffer(m_pWASFormat, reinterpret_cast<tbyte*>(pData), static_cast<tint>(numFramesAvailable));
			writeToAudioIOCallback(&buffer, systemTime);
			m_pAudioRenderClient->ReleaseBuffer(numFramesAvailable, 0);
		}
	}
}

//-------------------------------------------------------------------------------------------

void AOWin32::setCodecSampleFormatType(engine::Codec *codec, engine::RData *item)
{
	if(!item->isMixing())
	{
		if(m_deviceType==AOQueryDevice::Device::e_deviceASIO || (!m_pSampleConverter.isNull() && !m_pSampleConverter->isFloat()))
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
	else
	{
		codec->setDataTypeFormat(engine::e_SampleFloat);
	}
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
