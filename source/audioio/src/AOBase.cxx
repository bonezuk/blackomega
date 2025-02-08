#include "audioio/inc/AOBase.h"
#if defined(OMEGA_WIN32)
#include "audioio/inc/AOWin32.h"
#include "audioio/inc/AOQueryWin32.h"
#elif defined(OMEGA_MACOSX)
#include "audioio/inc/AOCoreAudio.h"
#include "audioio/inc/AOQueryCoreAudio.h"
#elif defined(OMEGA_LINUX)
#include "audioio/inc/AOQueryALSA.h"
#elif defined(OMEGA_IOS)
#include "audioio/inc/AOCoreAudioIOS.h"
#include "audioio/inc/AOQueryCoreAudioIOS.h"
#elif defined(OMEGA_ANDROID)

#endif

#include "common/inc/SBService.h"

#if defined(OMEGA_MACOSX)
#include <QDir>
#include <QCoreApplication>
#endif

#include "engine/dsdomega/inc/DSDCodec.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS_IMPL(AOFactory,AOBase)

//-------------------------------------------------------------------------------------------
// AOResampleInfo
//-------------------------------------------------------------------------------------------

AOResampleInfo::AOResampleInfo() : m_start(),
	m_end(),
	m_complete(false)
{}

//-------------------------------------------------------------------------------------------

AOResampleInfo::AOResampleInfo(const AOResampleInfo& rhs) : m_start(rhs.m_start),
	m_end(rhs.m_end),
	m_complete(rhs.m_complete)
{}

//-------------------------------------------------------------------------------------------

const AOResampleInfo& AOResampleInfo::operator = (const AOResampleInfo& rhs)
{
	if(this!=&rhs)
	{
		m_start = rhs.m_start;
		m_end = rhs.m_end;
		m_complete = rhs.m_complete;
	}
	return *this;
}

//-------------------------------------------------------------------------------------------
// AudioThread
//-------------------------------------------------------------------------------------------

AudioThread::AudioThread(const QString& name,QObject *parent) : QThread(parent),
	m_name(name),
	m_mutex(),
	m_condition(),
	m_audioOutput()
{}

//-------------------------------------------------------------------------------------------

AudioThread::~AudioThread()
{}

//-------------------------------------------------------------------------------------------

bool AudioThread::ignite()
{
	m_mutex.lock();
	start(QThread::HighPriority);
	m_condition.wait(&m_mutex);
	m_mutex.unlock();
    return (!m_audioOutput.isNull()) ? true : false;
}

//-------------------------------------------------------------------------------------------

void AudioThread::run()
{
	bool isLive;
    common::SBServicePtr sService = common::SBServiceFactory::createShared(SBFACTORYKEY);
	void *pPool = sService->allocatePool();
	
	{
		QSharedPointer<AOBase> liveAudioOutput(AOFactory::createShared(m_name));
        isLive = liveAudioOutput->isLive();
	}
	
	if(!isLive || AOBase::startAudioService())
	{
		QSharedPointer<AOBase> audioOutput(AOFactory::createShared(m_name));

		engine::Codec::setAudioThreadID();
		if(audioOutput->init())
		{
			m_audioOutput = audioOutput;
			QThread::yieldCurrentThread();
			m_condition.wakeAll();
			exec();
			m_audioOutput->stopCodec();
			m_audioOutput->reset();
			if(isLive)
			{
				AOBase::stopAudioService();
			}
			return;
		}
			
		if(isLive)
		{
			AOBase::stopAudioService();
		}
	}
	m_condition.wakeAll();
	sService->releasePool(pPool);
}

//-------------------------------------------------------------------------------------------

QSharedPointer<AOBase> AudioThread::audio()
{
	return m_audioOutput;
}

//-------------------------------------------------------------------------------------------
// AudioItem
//-------------------------------------------------------------------------------------------

AudioItem::AudioItem() : m_audioOutput(0),
	m_prev(0),
	m_next(0),
	m_done(0),
	m_state(e_stateEmpty),
	m_data(0)
{}

//-------------------------------------------------------------------------------------------

AudioItem::AudioItem(AOBase *ao,tint len,tint inChannel,tint outChannel) : m_audioOutput(ao),
	m_prev(0),
	m_next(0),
	m_done(0),
	m_state(e_stateEmpty),
	m_data(0)
{
	m_data = m_audioOutput->allocateData(len,inChannel,outChannel);
}

//-------------------------------------------------------------------------------------------

AudioItem::~AudioItem()
{
	if(m_data!=0)
	{
		delete m_data;
		m_data = 0;
	}
}

//-------------------------------------------------------------------------------------------

AudioItem *AudioItem::prev()
{
	return m_prev;
}

//-------------------------------------------------------------------------------------------

const AudioItem *AudioItem::prevConst() const
{
	return m_prev;
}

//-------------------------------------------------------------------------------------------

void AudioItem::setPrev(AudioItem *item)
{
	m_prev = item;
}

//-------------------------------------------------------------------------------------------

AudioItem *AudioItem::next()
{
	return m_next;
}

//-------------------------------------------------------------------------------------------

const AudioItem *AudioItem::nextConst() const
{
	return m_next;
}

//-------------------------------------------------------------------------------------------

void AudioItem::setNext(AudioItem *item)
{
	m_next = item;
}

//-------------------------------------------------------------------------------------------

AudioItem::ItemStates AudioItem::state() const
{
	return m_state;
}

//-------------------------------------------------------------------------------------------

void AudioItem::setState(AudioItem::ItemStates v)
{
	m_state = v;
}

//-------------------------------------------------------------------------------------------

tint AudioItem::done() const
{
	return m_done;
}

//-------------------------------------------------------------------------------------------

void AudioItem::setDone(tint v)
{
	m_done = v;
}

//-------------------------------------------------------------------------------------------

engine::AData *AudioItem::data()
{
	return m_data;
}

//-------------------------------------------------------------------------------------------

const engine::AData *AudioItem::dataConst() const
{
	return m_data;
}

//-------------------------------------------------------------------------------------------

void AudioItem::setData(engine::AData *d)
{
	m_data = d;
}

//-------------------------------------------------------------------------------------------
// AudioEvent
//-------------------------------------------------------------------------------------------

AudioEvent::AudioEvent(AudioEvent::AudioEventType t) : QEvent(static_cast<QEvent::Type>(t)),
	m_device(0),
	m_url(),
	m_time(),
	m_volume(1.0f),
	m_timeLength(),
	m_exclusive(false),
	m_isCallback(false)
{}

//-------------------------------------------------------------------------------------------

AudioEvent *AudioEvent::clone(AudioEvent *pEvent)
{
	AudioEvent *cEvent = new AudioEvent(static_cast<AudioEventType>(pEvent->type()));
	cEvent->uri() = pEvent->uri();
	cEvent->device() = pEvent->device();
	cEvent->time() = pEvent->time();
	cEvent->timeLength() = pEvent->timeLength();
	cEvent->volume() = pEvent->volume();
	cEvent->exclusive() = pEvent->exclusive();
	cEvent->isCallback() = pEvent->isCallback();
	return cEvent;
}

//-------------------------------------------------------------------------------------------

const QString& AudioEvent::uri() const
{
	return m_url;
}

//-------------------------------------------------------------------------------------------

QString& AudioEvent::uri()
{
	return m_url;
}

//-------------------------------------------------------------------------------------------

const tint& AudioEvent::device() const
{
	return m_device;
}

//-------------------------------------------------------------------------------------------

tint& AudioEvent::device()
{
	return m_device;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AudioEvent::time() const
{
	return m_time;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp& AudioEvent::time()
{
	return m_time;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AudioEvent::timeLength() const
{
	return m_timeLength;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp& AudioEvent::timeLength()
{
	return m_timeLength;
}

//-------------------------------------------------------------------------------------------

const sample_t& AudioEvent::volume() const
{
	return m_volume;
}

//-------------------------------------------------------------------------------------------

sample_t& AudioEvent::volume()
{
	return m_volume;
}

//-------------------------------------------------------------------------------------------

const bool& AudioEvent::exclusive() const
{
	return m_exclusive;
}

//-------------------------------------------------------------------------------------------

bool& AudioEvent::exclusive()
{
	return m_exclusive;
}

//-------------------------------------------------------------------------------------------

const bool& AudioEvent::isCallback() const
{
	return m_isCallback;
}

//-------------------------------------------------------------------------------------------

bool& AudioEvent::isCallback()
{
	return m_isCallback;
}

//-------------------------------------------------------------------------------------------
// AOBase
//-------------------------------------------------------------------------------------------

tint AOBase::m_audioStartCount = 0;
tint AOBase::m_defaultDeviceIndex = 0;
AOQueryDevice *AOBase::m_deviceInfo = 0;
QRecursiveMutex AOBase::m_deviceInfoMutex;

//-------------------------------------------------------------------------------------------

AOBase::AOBase(QObject *parent) : QObject(parent),
	m_state(e_stateStop),
	m_noOfCyclicBufferItems(0),
	m_noInChannels(2),
	m_noOutChannels(2),
	m_noChannels(2),
	m_frequency(44100),
	m_codecFrequency(44100),
	m_thread(0),
	m_timer(0),
	m_codecState(e_codecNoPlay),
	m_codec(0),
	m_nextCodec(0),
	m_completeCodec(0),
	m_callbackAudioItem(0),
	m_codecAudioItem(0),
	m_deviceIdx(m_defaultDeviceIndex),
	m_currentPlayTime(),
	m_audioStartClock(),
	m_currentOutTime(),
	m_pauseTime(),
	m_codecCurrentTime(),
	m_codecTimeLength(),
	m_codecTimePositionComplete(),
	m_nextCodecTime(),
	m_nextCodecTimeLengthComplete(),
	m_crossFadeTime(),
	m_frameFadeTime(),
	m_crossFadeTimeLen(),
	m_progFadeTime(),
	m_outputLatencyTime(),
	m_audioStartFlag(false),
	m_mutexCount(0),
	m_trackTimeState(0),
	m_trackTimeStateFlag(false),
	m_stopTimeClock(),
	m_stopTimeFlag(false),
	m_progFadeState(0),
	m_audioProcessType(0),
	m_nextOutState(0),
	m_nextName(),
	m_volume(1.0f),
	m_pauseAudioFlag(false),
	m_codecTimeLengthUpdate(false),
	m_crossFadeAWin(0),
	m_crossFadeBWin(0),
	m_crossASample(0),
	m_crossBSample(0),
	m_crossFadeItem(0),
	m_crossFadeFlag(false),
	m_startNextTrackFlag(false),
	m_syncAudioToTimestamp(false),
	m_silenceIsWritten(false),
	m_resampleFlag(false),
	m_resampleRatio(1.0),
	m_resampleItem(0),
	m_rInFrequency(0),
	m_rUsedI(0),
	m_rUsedO(0),
	m_rSrcLen(0),
	m_rDstLen(0),
	m_rOutNo(0),
	m_rOutDrift(0.0),
	m_rCodecCompleteFlag(false),
	m_resampleList(),
	m_nextCodecSeekTime(),
	m_startCodecSeekTime(),
	m_mergeCodec(0),
	m_mergeAudioItem(0),
	m_mergeCodeTime(),
	m_mergeCurrentPlayTime(),
	m_mergeNextPlayTime(10.0),
	m_eventQueueTimer(0),
	m_eventQueueMutex(),
	m_eventQueue(),
	m_recursiveSlotList(),
	m_forceBitsPerSample(-1),
	m_lfeFilter()
{
	::memset(m_resample,0,sizeof(engine::Resample*) * 8);
	::memset(m_rIn,0,sizeof(sample_t *) * 8);
	::memset(m_rOut,0,sizeof(sample_t *) * 8);
#if defined(DEBUG_LOG_AUDIOOUTPUT)
	m_debugList = 0;
#endif	
}

//-------------------------------------------------------------------------------------------

AOBase::~AOBase()
{
	if(m_crossFadeAWin!=0)
	{
		delete [] m_crossFadeAWin;
		m_crossFadeAWin = 0;
	}
	if(m_crossFadeBWin!=0)
	{
		delete [] m_crossFadeBWin;
		m_crossFadeBWin = 0;
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::printError(const tchar *strR,const tchar *strE) const
{
	common::Log::g_Log << "AOBase::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void AOBase::printToLog(const tchar *msg) const
{
	common::Log::g_Log << msg << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool AOBase::startAudioService()
{
	int i;
	bool res = true;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::startAudioService\n");
#endif

	if(!m_audioStartCount)
	{
#if defined(OMEGA_WIN32)
		if(!AOWin32::startAudioService())
		{
			res = false;
		}
#endif
		if(res)
		{
#if defined(OMEGA_WIN32)
			m_deviceInfo = new AOQueryWin32;
#elif defined(OMEGA_MACOSX)
			m_deviceInfo = new AOQueryCoreAudio;
#elif defined(OMEGA_LINUX)
            m_deviceInfo = new AOQueryALSA;
#elif defined(OMEGA_IOS)
			m_deviceInfo = new AOQueryCoreAudioIOS;
#elif defined(OMEGA_ANDROID)
#error AOQuery for Android must be defined
#endif

			m_deviceInfoMutex.lock();
			if(m_deviceInfo->queryNames())
			{
				QString defaultDeviceID,defaultDeviceName;
				QSettings settings;
				bool found = false;
				
				for(i=0;i<m_deviceInfo->noDevices();i++)
				{
					m_deviceInfo->queryDevice(i);
					m_deviceInfo->deviceDirect(i)->loadChannelMap();
				}

#if defined(OMEGA_DEBUG)
//				m_deviceInfo->print();
#endif

				settings.beginGroup("audio");
				if(settings.contains(QString::fromLatin1("defaultDeviceID")))
				{
					defaultDeviceID = settings.value(QString::fromLatin1("defaultDeviceID"),QVariant(QString())).toString();
					if(!defaultDeviceID.isEmpty())
					{
						for(i=0;i<m_deviceInfo->noDevices() && !found;i++)
						{
							if(defaultDeviceID==m_deviceInfo->device(i).idConst())
							{
								m_defaultDeviceIndex = i;
								found = true;
							}
						}						
					}
				}
				if(!found)
				{
					if(settings.contains(QString::fromLatin1("defaultDeviceName")))
					{
						defaultDeviceName = settings.value(QString::fromLatin1("defaultDeviceName"),QVariant(QString())).toString();
						if(!defaultDeviceName.isEmpty())
						{
							for(i=0;i<m_deviceInfo->noDevices() && !found;i++)
							{
								if(defaultDeviceName==m_deviceInfo->device(i).name())
								{
									m_defaultDeviceIndex = i;
									found = true;
								}
							}
						}
					}
				}
				if(!found)
				{
					m_defaultDeviceIndex = m_deviceInfo->defaultDeviceIndex();
				}
				if(!(m_defaultDeviceIndex>=0 && m_defaultDeviceIndex<m_deviceInfo->noDevices()))
				{
					res = false;
				}
				settings.endGroup();
			}
			else
			{
				res = false;
			}
			m_deviceInfoMutex.unlock();
		}
	}
	
	if(res)
	{
		m_audioStartCount++;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void AOBase::stopAudioService()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::stopAudioService\n");
#endif

	m_audioStartCount--;
	if(!m_audioStartCount)
	{
		m_deviceInfoMutex.lock();
		if(m_deviceInfo!=0)
		{
			delete m_deviceInfo;
			m_deviceInfo = 0;
		}
		m_deviceInfoMutex.unlock();
#if defined(OMEGA_WIN32)
		AOWin32::stopAudioService();
#endif
	}
}

//-------------------------------------------------------------------------------------------

bool AOBase::isLive() const
{
	return true;
}

//-------------------------------------------------------------------------------------------

bool AOBase::init()
{
	QSettings settings;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::init\n");
#endif

	m_threadId = QThread::currentThreadId();
		
	settings.beginGroup("audio");
	if(settings.contains(QString::fromLatin1("crossfade")))
	{
		m_crossFadeTimeLen = settings.value(QString::fromLatin1("crossfade"),QVariant(0.0)).toDouble();
		m_crossFadeFlag = (m_crossFadeTimeLen!=0) ? true : false;
	}
	else
	{
		m_crossFadeTimeLen = 0;
		m_crossFadeFlag = false;
	}
	settings.endGroup();
	
	initCrossFadeWindow();
	
	m_timer = new QTimer(this);
	QObject::connect(m_timer,SIGNAL(timeout()),this,SLOT(onTimer()));
	m_eventQueueTimer = new QTimer(this);
	QObject::connect(m_eventQueueTimer,SIGNAL(timeout()),this,SLOT(onEventTimer()));
	m_eventQueueTimer->start(100);
	
	return true;
}

//-------------------------------------------------------------------------------------------

void AOBase::reset()
{
	QSettings settings;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::reset\n");
#endif

	settings.beginGroup("audio");
	settings.setValue("crossfade",QVariant(static_cast<tfloat64>(m_crossFadeTimeLen)));
	settings.endGroup();
	
	if(m_eventQueueTimer!=0)
	{
		m_eventQueueTimer->stop();
	}
}

//-------------------------------------------------------------------------------------------

QSharedPointer<AOBase> AOBase::get(const QString& type)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::get - %s\n", type.toUtf8().constData());
#endif

	AudioThread *aThread = new AudioThread(type);
	if(aThread->ignite())
	{
		aThread->audio()->m_thread = aThread;
		return QSharedPointer<AOBase>(aThread->audio());
	}
	else
	{
		delete aThread;
		return QSharedPointer<AOBase>();
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::end(QSharedPointer<AOBase> audioOutput)
{
	AudioThread *aThread = audioOutput->m_thread;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::end\n");
#endif

	if(aThread!=0)
	{
		aThread->quit();
		aThread->wait();
		delete aThread;
	}
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::getCacheTimeLength() const
{
	common::TimeStamp len;
	QSettings settings;

	settings.beginGroup("buffer");
	if(settings.contains("lengthCache"))
	{
		len = settings.value("lengthCache", QVariant(2.0)).toDouble();
	}
	else
	{
		len = 2.0;
	}
	settings.endGroup();
	return len;
}

//-------------------------------------------------------------------------------------------

void AOBase::initCyclicBuffer()
{
	tint i, noItems, noSamplesPerAudioItem;
	AudioItem *item = 0,*pItem=0;
	
	freeCyclicBuffer();
	
	noSamplesPerAudioItem = c_noSamplesPerAudioItem;
	while(noSamplesPerAudioItem < engine::minimumAudioItemLengthFor200HzLowPass(getFrequency()))
	{
		noSamplesPerAudioItem <<= 1;
	}
	
	tfloat64 bufferT = static_cast<tfloat64>(getCacheTimeLength());
	noItems = static_cast<tint>(ceil((static_cast<tfloat64>(m_frequency)* bufferT) / static_cast<tfloat64>(noSamplesPerAudioItem)));
	if(noItems < 3)
	{
		noItems = 3;
	}
	m_noOfCyclicBufferItems = noItems;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::initCyclicBuffer - %d, %d, %.8f,\n", noItems, m_frequency, static_cast<tfloat64>(bufferT));
#endif

	for(i=0;i<noItems;++i)
	{
		item = new AudioItem(this,noSamplesPerAudioItem,m_noInChannels,m_noOutChannels);
		
		if(pItem==0)
		{
			m_callbackAudioItem = item;
		}
		else
		{
			pItem->setNext(item);
		}
		item->setPrev(pItem);
		pItem = item;
	}
	if(item!=0)
	{
		item->setNext(m_callbackAudioItem);
		m_callbackAudioItem->setPrev(item);
	}
	m_codecAudioItem = m_callbackAudioItem;
	
	m_crossFadeItem = new AudioItem(this,noSamplesPerAudioItem,m_noInChannels,m_noOutChannels);
		
	m_crossASample = new sample_t[noSamplesPerAudioItem * m_noOutChannels];
	m_crossBSample = new sample_t[noSamplesPerAudioItem * m_noOutChannels];
	
	m_mergeAudioItem = new AudioItem(this,noSamplesPerAudioItem,m_noInChannels,m_noOutChannels);
}

//-------------------------------------------------------------------------------------------

void AOBase::freeCyclicBuffer()
{
	AudioItem *nItem,*item = m_callbackAudioItem;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::freeCyclicBuffer\n");
#endif

	if(item!=0)
	{
		do
		{
			nItem = item->next();
			delete item;
			item = nItem;
		} while(item!=m_callbackAudioItem);
	}
	m_callbackAudioItem = 0;
	m_codecAudioItem = 0;
	
	if(m_crossFadeItem!=0)
	{
		delete m_crossFadeItem;
		m_crossFadeItem = 0;
	}
	if(m_crossASample!=0)
	{
		delete [] m_crossASample;
		m_crossASample = 0;
	}
	if(m_crossBSample!=0)
	{
		delete [] m_crossBSample;
		m_crossBSample = 0;
	}
	if(m_mergeAudioItem!=0)
	{
		delete m_mergeAudioItem;
		m_mergeAudioItem = 0;
	}

	m_noOfCyclicBufferItems = 0;

	resetLFEChannel();
}

//-------------------------------------------------------------------------------------------

QString AOBase::getActiveDeviceName()
{
	return getDeviceName(m_deviceIdx);
}

//-------------------------------------------------------------------------------------------

QString AOBase::getDeviceName(tint devIdx)
{
	QString devName;

	m_deviceInfoMutex.lock();
	if(devIdx>=0 && devIdx<m_deviceInfo->noDevices())
	{
		devName = m_deviceInfo->device(devIdx).name();
	}
	m_deviceInfoMutex.unlock();
	
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::getDeviceName - %d, %s\n", devIdx, devName.toUtf8().constData());
#endif
	
	return devName;
}

//-------------------------------------------------------------------------------------------

void AOBase::flushCyclicBuffer()
{
	AudioItem *item = m_callbackAudioItem;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::flushCyclicBuffer\n");
#endif
	
	do
	{
		item->setState(AudioItem::e_stateEmpty);
		item->setDone(0);
		item->data()->reset();
		item = item->prev();
	} while(item!=m_callbackAudioItem);
	
	m_callbackAudioItem = item;
	m_codecAudioItem = item;
	
	if(m_crossFadeItem!=0)
	{
		m_crossFadeItem->setDone(0);
		m_crossFadeItem->data()->reset();
	}
}

//-------------------------------------------------------------------------------------------

engine::Codec *AOBase::createNewCodecFromUrl(const QString& url) const
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::createNewCodecFromUrl - %s\n", url.toUtf8().constData());
#endif

	return engine::Codec::get(url);
}

//-------------------------------------------------------------------------------------------

void AOBase::connectPreBufferedRemoteCodec()
{
	common::TimeStamp bT;
	
	QObject::connect(getCodec(),SIGNAL(onInit(void*)),this,SLOT(onCodecInit(void*)));
	bT = getRemoteTimeSync();

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::connectPreBufferedRemoteCodec - %.8f\n", static_cast<tfloat64>(bT));
#endif

	getCodec()->buffer(bT);
	setState(e_statePreBuffer);
	startInternalTimer(100);
}

//-------------------------------------------------------------------------------------------

void AOBase::startInternalTimer(int period)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::startInternalTimer - %d\n", period);
#endif
	m_timer->start(period);
}

//-------------------------------------------------------------------------------------------

void AOBase::emitOnStart(const QString& url)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::emitOnStart - %s\n", url.toUtf8().constData());
#endif
	emit onStart(url);
}

//-------------------------------------------------------------------------------------------

void AOBase::emitOnPause()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::emitOnPause\n");
#endif
	emit onPause();
}

//-------------------------------------------------------------------------------------------

void AOBase::emitOnPlay()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::emitOnPlay\n");
#endif
	emit onPlay();
}

//-------------------------------------------------------------------------------------------

void AOBase::startPlayTimeWithSingleCodec()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::startPlayTimeWithSingleCodec\n");
#endif
	m_codecState = e_codecSingle;
	m_currentPlayTime.set(0);
}

//-------------------------------------------------------------------------------------------

void AOBase::connectPreBufferedNextRemoteCodec(const QString& url,bool fade)
{
	common::TimeStamp bT;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::connectPreBufferedNextRemoteCodec\n");
#endif

	QObject::connect(getNextCodec(),SIGNAL(onInit(void*)),this,SLOT(onCodecInit(void*)));
	bT = getRemoteTimeSync();
	getNextCodec()->buffer(bT);
	m_refStartAudioTime = getReferenceClockTime() + bT;
	m_nrCrossfadeFlag = fade;
	m_nextName = url;
}

//-------------------------------------------------------------------------------------------

void AOBase::calculateNextCodecCrossFadeTime()
{
	common::TimeStamp eC;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::connectPreBufferedNextRemoteCodec\n");
#endif

	setCrossFadeTime(getCodecCurrentTime() + 0.01);
	eC = getCrossFadeTime() + getProgFadeTime();
	if(eC < getCodecTimeLength() || (getCodecTimePositionComplete()>0 && eC<getCodecTimePositionComplete()))
	{
		setCodecTimeLength(eC);
	}
	setCodecTimeLengthUpdate(false);
	setNextOutState(1);
}

//-------------------------------------------------------------------------------------------

void AOBase::emitOnNoNext()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::emitOnNoNext\n");
#endif
	emit onNoNext();
}

//-------------------------------------------------------------------------------------------

bool AOBase::startCodec(const QString& url,const common::TimeStamp& t,const common::TimeStamp& tLen)
{
	bool res = false;
	
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::startCodec - %s, %.8f\n",url.toUtf8().constData(),static_cast<tfloat64>(t));
#endif
	stopCodec(false);
	
	if(getState()==e_stateStop && getCodecState()==e_codecNoPlay)
	{
		setCodec(createNewCodecFromUrl(url));
		if(getCodec()!=0)
		{
			setStartCodecSeekTime(t);
			
			if(tLen>0)
			{
				common::TimeStamp tEnd = t + tLen;
				setCodecTimePositionComplete(tEnd);
			}
			else
			{
                common::TimeStamp zeroLenT(0);
				setCodecTimePositionComplete(zeroLenT);
			}
			
			if(canAudioFromCodecBePlayed())
			{
				getCodec()->setNoOutputChannels(getNoChannelsMapped());
				startPlayTimeWithSingleCodec();
				
				if(getCodec()->isRemote())
				{
					connectPreBufferedRemoteCodec();
					emitOnStart(url);
					res = true;
				}
				else
				{
					if(startAudio(url))
					{
						startInternalTimer(100);
						res = true;
					}
					else
					{
						printError("startCodec","Error initializing audio device");
						emitOnStart(QString());
					}
				}
			}
			else
			{
				res = false;
			}
		}
		else
		{
			common::BString err("Could not find compitable codec for '");
			err += url.toUtf8().constData();
			err += "'";
			printError("startCodec",static_cast<const tchar *>(err));
			emitOnStart(QString());
		}
	}
	else
	{
		printError("startCodec","Audio state is not in stop state");
		emitOnStart(QString());
	}
	
	if(!res)
	{
		stopCodec(true);
	}
	return res;	
}

//-------------------------------------------------------------------------------------------

void AOBase::stopCodec(bool eFlag)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::stopCodec - 1\n");
#endif

	m_timer->stop();
	stopAudio(eFlag);
	switch(m_codecState)
	{
		case e_codecSingle:
		case e_codecNextQueued:
			{
				if(m_codec!=0)
				{
					if(m_codec->isRemote())
					{
						QObject::disconnect(m_codec,SIGNAL(onInit(void*)),this,SLOT(onCodecInit(void*)));
					}
				}
				m_codec->close();
				delete m_codec;
				m_codec = 0;
			}
			break;

		case e_codecSingleFinish:
		case e_codecCurrentFinish:
			{
				if(m_completeCodec!=0)
				{
					if(m_completeCodec->isRemote())
					{
						QObject::disconnect(m_completeCodec,SIGNAL(onInit(void*)),this,SLOT(onCodecInit(void*)));
					}
				}
				m_completeCodec->close();
				delete m_completeCodec;
				m_completeCodec = 0;
			}
			break;
			
		default:
			break;
	}
	stopNextCodec();
	freeCyclicBuffer();
	m_state = e_stateStop;
	m_codecState = e_codecNoPlay;
}

//-------------------------------------------------------------------------------------------
// 
// startNextCodec implementation notes - If another codec is already queued it is removed. If no codec is currently 
// now being processed then the startCodec method is used instead. It could be that by the time this method is called
// the codec has finished processing and seen to be completed, this would lead to the audio being stop and started and 
// hence not seamless.
// 
// Given that any existing next codec has been dequeued the given url is opened and its start is set accordingly. If
// the file is offset then a seek is done. This would imply that the initial caching and the associated read commands
// would need to be cancelled and new ones issued.
// 
// The current codec being processed is checked. However it is held until the complete process is done so this should
// be checked as well if the current codec has finished processing. Or the audio device settings should be made
// available or checked through a virtual method implemented on a per audio device basis (considering that bit depth
// also has to be considered).
// 
// The next codec is considered to be compatible if its frequency and no of input channels are. If it is not then the
// process of closing down and restarting the audio device is carried out. Another factor is bit depth. As each codec
// will be associated with a given bit depth and thus a change in bit depth may require the audio device to be reset 
// such that it reflects the rendering of this new bit depth, based on the DACs capabilities, the codec should be tested
// to see if the current DAC setup is optimal or not.
// 
// As frequency and number of channels define the size of memory allocated to the cyclic buffer and hence the representation
// by its length in units of time, and because these are used to determine if the current audio device setup is comptabile
// then no mechanism of "respooling" or "rethreading" the active cyclic buffer has to occur.
// 
// Given that the next codec is comptabile with the current open audio device settings then: If the crossfade is active
// then its cross over point is defined and setup accordingly. If the current codec has finished processing then the
// next codec is immediately put in place as the current so that processing can start happening on it, otherwise it is
// queued as the next to be processed.
// 
// If the next codec cannot be queued as the next for seamless audio to work then the onNoNext signal is emitted such that
// the next codec should be queued in a different manner such that audio device and cyclic buffer are reinitialised.
// Maybe the onNoNext signal should only be given if the current codec cannot be opened and initialised. If the next codec
// is incomptabile then it should still be queued but the processing state changed such that the reinitialisation process
// happens.
//
//-------------------------------------------------------------------------------------------

bool AOBase::startNextCodec(const QString& url,const common::TimeStamp& nT,const common::TimeStamp& nTLen,bool fade)
{
	bool res = false;
	
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::startNextCodec - %s, %.8f\n",url.toUtf8().constData(),static_cast<tfloat64>(nT));
#endif

	stopNextCodec();
	if(getCodecState()==e_codecNoPlay)
	{
		return startCodec(url,nT,nTLen);
	}

	if(getNextCodec()==0)
	{
		setNextCodec(createNewCodecFromUrl(url));
		if(getNextCodec()!=0)
		{	
			getNextCodec()->setNoOutputChannels(getNoChannelsMapped());
			
			setNextCodecSeekTime(nT);
			setNextCodecTimeLengthComplete(nTLen);
			
			if(!getNextCodec()->isRemote())
			{
				if(getNextCodec()->init())
				{
					if(getNextCodec()->isSeek() && getNextCodecSeekTime()>0)
					{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
						common::Log::g_Log.print("AOBase::startNextCodec - Seek to start of given track position\n");
#endif
						getNextCodec()->seek(getNextCodecSeekTime());
					}

					if(isNextCodecSeamless())
					{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
						common::Log::g_Log.print("AOBase::startNextCodec - Next codec can be played without gaps\n");
#endif

						if(fade && getState()==e_statePlay)
						{
							calculateNextCodecCrossFadeTime();
						}
						setNextName(url);
	
						if(getCodecState()==e_codecSingleFinish)
						{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
							common::Log::g_Log.print("AOBase::startNextCodec - Previous has finished\n");
#endif

							setCodecState(e_codecCurrentFinish);
							setCodec(getNextCodec());
							setNextCodec(0);
							
							setStartCodecSeekTime(nT);
							if(nTLen > 0)
							{
								common::TimeStamp tEnd = nT + nTLen;
								setCodecTimePositionComplete(tEnd);
							}
							else
							{
								common::TimeStamp zeroLenT(0);
								setCodecTimePositionComplete(zeroLenT);
							}
							
							calcNextCodecTime();
							setState(e_statePlay);
						}
						else
						{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
							common::Log::g_Log.print("AOBase::startNextCodec - Next codec is queued for playback\n");
#endif
							setCodecState(e_codecNextQueued);
						}
						res = true;
					}
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
					else
					{
						if(getCodec()!=0)
						{
							common::Log::g_Log.print("AOBase::startNextCodec - Gapless skip. Current codec %dHz, %dChs. Next codec %dHz, %dChs.\n",getCodec()->frequency(),getNoInChannels(),getNextCodec()->frequency(),getNextCodec()->noChannels());
						}
						else
						{
							common::Log::g_Log.print("AOBase::startNextCodec - Gapless skip as no current codec\n");
						}
					}
#endif

				}
				else
				{
					common::BString err("Failed to initialized codec for '");
					err += url.toUtf8().constData();
					err += "'";
					printError("startNextCodec",static_cast<const tchar *>(err));
				}
			}
			else
			{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
				common::Log::g_Log.print("AOBase::startNextCodec - Is remote\n");
#endif
				connectPreBufferedNextRemoteCodec(url,fade);
				res = true;
			}
		}
		else
		{
			common::BString err("Failed to open codec for '");
			err += url.toUtf8().constData();
			err += "'";
			printError("startNextCodec",static_cast<const tchar *>(err));
		}
	}
	else
	{
		printError("startNextCodec","Next codec already defined");
	}
	
	if(!res)
	{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
		common::Log::g_Log.print("AOBase::startNextCodec - Failed to start next codec\n");
#endif

		stopNextCodec();
		if(!getNextOutState())
		{
			setNextOutState(2);
			emitOnNoNext();
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOBase::stopCodecDoNext()
{
	bool res;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::stopCodecDoNext\n");
#endif

	if(m_nextCodec!=0)
	{
		resetResampler(m_nextCodec->frequency(),m_frequency);
	}
	else
	{
		resetResampler();
	}

	if(m_completeCodec!=0)
	{
		if(m_completeCodec->isRemote())
		{
			QObject::disconnect(m_completeCodec,SIGNAL(onInit(void*)),this,SLOT(onCodecInit(void*)));
		}
		delete m_completeCodec;
	}
	m_completeCodec = m_codec;
	m_codec = 0;

	setCodecCompletePositionFromNext();

	if(m_nextCodec!=0)
	{
		m_codec = m_nextCodec;
		m_nextCodec = 0;
		
		calcNextCodecTime();
		
		if(m_codec->isRemote())
		{
			tfloat32 percent = 0.0f;
			m_state = (m_codec->isBuffered(percent)) ? e_statePlay : e_statePreBuffer;
		}
		else
		{
			m_state = e_statePlay;
		}
		m_codecState = (m_completeCodec!=0) ? e_codecCurrentFinish : e_codecSingle;
		res = true;
	}
	else
	{
		m_state = e_stateNoCodec;
		m_codecState = (m_completeCodec!=0) ? e_codecSingleFinish : e_codecNoPlay;
		res = false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCodecCompletePositionFromNext()
{
	common::TimeStamp zeroPos;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::setCodecCompletePositionFromNext\n");
#endif

	if(getNextCodec()!=0 && getNextCodecTimeLengthComplete()>0)
	{
		common::TimeStamp endPos = getNextCodecSeekTime() + getNextCodecTimeLengthComplete();
		setCodecTimePositionComplete(endPos);
	}
	else
	{
		setCodecTimePositionComplete(zeroPos);
	}
	setNextCodecTimeLengthComplete(zeroPos);
}

//-------------------------------------------------------------------------------------------

void AOBase::stopNextCodec()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::stopNextCodec\n");
#endif

	if(m_nextCodec!=0)
	{
		if(m_nextCodec->isRemote())
		{
			QObject::disconnect(m_nextCodec,SIGNAL(onInit(void*)),this,SLOT(onCodecInit(void*)));
		}
		m_nextCodec->close();
		delete m_nextCodec;
		m_nextCodec = 0;
	}
	if(m_codec!=0)
	{
		m_codecState = e_codecSingle;
	}
	else if(m_completeCodec!=0)
	{
		m_codecState = e_codecSingleFinish;
	}
	else
	{
		m_codecState = e_codecNoPlay;
	}
}

//-------------------------------------------------------------------------------------------

bool AOBase::startAudio(const QString& url)
{
	bool res = false;
	
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::startAudio - %s\n",url.toUtf8().constData());
#endif

	if(m_codec!=0)
	{
		if(m_codec->init())
		{
			if(m_startCodecSeekTime>0.0)
			{
				m_codec->seek(m_startCodecSeekTime);
			}
			m_currentOutTime = m_startCodecSeekTime;
			m_currentPlayTime.set(m_startCodecSeekTime);
			m_currentCallbackTime.set(m_startCodecSeekTime);
			m_pauseTime = m_startCodecSeekTime;
			m_startCodecSeekTime = 0.0;

			m_frequency = m_codecFrequency = m_codec->frequency();
			m_noInChannels = m_codec->noChannels();
			if(m_noInChannels!=2)
			{
				m_noOutChannels = getNoChannelsMapped();
			}
			else
			{
				m_noOutChannels = 2;
			}
			buildChannelMapArray();
			
			calcNextCodecTime();
			
			m_state = e_statePlay;
			
			if(openAudio())
			{
				m_audioStartFlag = false;
				m_silenceIsWritten = false;
				m_mutexCount = 0;

				emit onStart(url);

				processCodec();
				
				if(startAudioDevice())
				{
					res = true;
				}
				else
				{
					printError("startAudio","Failed to start audio device playback");
				}
			}
			else
			{
				printError("startAudio","Failed to open audio device");
			}
		}
		else
		{
			printError("startAudio","Failed to initialize audio codec");
		}
	}
	else
	{
		printError("startAudio","File stream and codec must be allocated first");
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void AOBase::stopAudio(bool eFlag)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::stopAudio\n");
#endif

	closeAudio();
	closeMergeCodec();

	m_audioStartFlag = false;

	if(eFlag && !(m_state==e_stateStop))
	{
		emit onStop();
	}
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::currentPlayTime()
{
	tint count;
	common::TimeStamp t;
	bool loop = true;
	
	while(loop)
	{
		count = static_cast<tint>(m_mutexCount);
		if(!(count & 0x00000001))
		{
			t = const_cast<const common::TimeStamp &>(m_currentPlayTime);
			loop = false;
		}
	};
	
	if(t >= m_outputLatencyTime)
	{
		t -= m_outputLatencyTime;
	}
	else
	{
		t = 0;
	}
	
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::currentPlayTime - %.8f\n", static_cast<tfloat64>(t));
#endif
	
	return t;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::currentCallbackTime()
{
	tint count;
	common::TimeStamp t;
	bool loop = true;
	
	while(loop)
	{
		count = static_cast<tint>(m_mutexCount);
		if(!(count & 0x00000001))
		{
			t = const_cast<const common::TimeStamp &>(m_currentCallbackTime);
			loop = false;
		}
	};
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::currentCallbackTime - %.8f\n", static_cast<tfloat64>(t));
#endif
	return t;
}

//-------------------------------------------------------------------------------------------

void AOBase::processCodec(bool initF)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodec - %d\n", (int)initF);
#endif

	if(getCodec()!=0)
	{
		AudioItem *item = getCodecAudioItem();
		if(item!=0)
		{
			bool loop = true;
			
			while(loop && (item->state()==AudioItem::e_stateEmpty || item->state()==AudioItem::e_stateCodec))
			{
				item = processCodecLoop(item,initF,loop);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

AudioItem *AOBase::processCodecLoop(AudioItem *item,bool& initF,bool& loop)
{
	AudioItem **pItem = &item;
	common::TimeStamp currentT;
	
	currentT = currentCallbackTime();

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecLoop - %.8f\n", static_cast<tfloat64>(currentT));
#endif

	if(getAudioProcessType()==0)
	{
		item->setState(AudioItem::e_stateCodec);
		item->data()->reset();
	}
	
	loop = processCodecState(pItem,currentT,initF);
	item = *pItem;
	
	if(loop && item->state()!=AudioItem::e_stateCodec)
	{
		item = item->next();
		setCodecAudioItem(item);
	}
	return item;
}

//-------------------------------------------------------------------------------------------

bool AOBase::processCodecState(AudioItem **pItem,const common::TimeStamp& currentT,bool& initF)
{
	AudioItem *item = *pItem;
	bool loop;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecState - %.8f\n", static_cast<tfloat64>(currentT));
#endif

	switch(getState())
	{
		case e_statePlay:
			loop = processCodecPlay(pItem,currentT,initF);
			break;
		
		case e_stateCrossFade:
			loop = processCodecCrossFade(item,currentT,initF);
			break;
		
		case e_statePreBuffer:
			loop = processCodecPreBuffer();
			break;
		
		case e_stateNoCodec:
		case e_statePause:
		case e_stateStop:
		default:
			loop = false;
			break;
	}
	return loop;
}

//-------------------------------------------------------------------------------------------

bool AOBase::processCodecPlay(AudioItem **pItem,const common::TimeStamp& currentT,bool& initF)
{
	AudioItem *item = *pItem;
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());
	tint iFrom = (getAudioProcessType()==2) ? data->noParts() : 0;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPlay - %.8f\n", static_cast<tfloat64>(currentT));
#endif

	bool flag = processCodecPlayDecode(item,currentT,initF);
	processCodecReadyForNext(item,flag,iFrom);
	return processCodecPlayPostProcess(pItem,currentT,flag);
}

//-------------------------------------------------------------------------------------------

bool AOBase::processCodecPlayDecode(AudioItem* item,const common::TimeStamp& currentT,bool& initF)
{
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());
	bool flag;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPlayDecode - %.8f\n", static_cast<tfloat64>(currentT));
#endif

	if(getAudioProcessType()!=1)
	{
		if(!getTrackTimeState())
		{
			flag = decodeAndResample(getCodec(),item,initF);
		}
		else
		{
			flag = processCodecPlayDecodeInTime(item,currentT,initF);
		}
		setCodecCurrentTime(data->part(data->noParts() - 1).end());
		processCodecPlayTagPartAsRequired(data);
		flag = processCodecEndForTimePositionComplete(item,flag);
	}
	else
	{
		flag = true;
	}
	setAudioProcessType(0);
	return flag;
}

//-------------------------------------------------------------------------------------------

bool AOBase::processCodecPlayDecodeInTime(AudioItem *item,const common::TimeStamp& currentT,bool& initF)
{
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());
	bool flag;
	
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPlayDecodeInTime - %.8f\n", static_cast<tfloat64>(currentT));
#endif
	
	while(flag = decodeAndResample(getCodec(),item,initF),flag)
	{
		if(data->noParts()==0)
		{
			break;
		}
		else if(currentT < data->part(data->noParts() - 1).end())
		{
			break;
		}
		data->reset();
	}
	return flag;
}

//-------------------------------------------------------------------------------------------

bool AOBase::processCodecEndForTimePositionComplete(AudioItem *item,bool decodeFlag)
{
	bool runningFlag;
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecEndForTimePositionComplete\n");
#endif

	if(getCodecTimePositionComplete()>0 && data->end()>=getCodecTimePositionComplete())
	{
		data->clipToTime(getCodecTimePositionComplete());
		runningFlag = false;
	}
	else
	{
		runningFlag = true;
	}
	return (decodeFlag && runningFlag);
}

///-------------------------------------------------------------------------------------------

void AOBase::processCodecPlayTagPartAsRequired(engine::RData *data)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPlayTagPartAsRequired\n");
#endif

	if(getAudioProcessType()==2)
	{
		data->part(data->noParts() - 1).setNext(true);
		if(getCodec()->isRemote())
		{
			common::TimeStamp zeroT = 0;
			data->part(data->noParts() - 1).refStartTime() = getRefStartAudioTime();
			setRefStartAudioTime(zeroT);
		}
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::processCodecReadyForNext(AudioItem *item,bool completeFlag,tint iFrom)
{
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecReadyForNext\n");
#endif

	if(data->noParts()>0)
	{
		common::TimeStamp endT = data->part(data->noParts() - 1).end();
		if(!completeFlag || (endT > getCodecTimeLength()))
		{
			if(getCodecTimeLengthUpdate())
			{
				setCodecTimeLength(endT);
			}
		}
		
		processCodecPlayNextEndInParts(data,completeFlag,iFrom);
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::processCodecPlayNextEndInParts(engine::RData *data,bool completeFlag,tint iFrom)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPlayNextEndInParts\n");
#endif

	if(getAudioProcessType()!=2)
	{
		for(tint i=iFrom;i<data->noParts();++i)
		{
			engine::RData::Part& pt = data->part(i);
		
			if(completeFlag && getCodecTimeLength()<pt.end() && getCodecTimeLengthUpdate())
			{
				setCodecTimeLength(pt.end());
			}
			
			switch(getNextOutState())
			{
				case 0:
					processCodecPlayNextOutStateZero(pt);
					break;
			
				case 1:
					processCodecPlayNextOutStateOne(pt,data);
					break;
					
				default:
					break;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::processCodecPlayNextOutStateZero(engine::RData::Part& part)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPlayNextOutStateZero\n");
#endif

	if(part.start() > getNextCodecTime())
	{
		setNextOutState(1);
		if(getNextCodec()==0)
		{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
			common::Log::g_Log.print("AOBase::processCodecPlayNextOutStateZero() - emit signal emitOnReadyForNext()\n");
#endif
			emitOnReadyForNext();
		}
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::processCodecPlayNextOutStateOne(engine::RData::Part& part,engine::RData *data)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPlayNextOutStateOne\n");
#endif

	if(getCrossFadeFlag() && getNextCodec()!=0 && part.end()>=getCrossFadeTime())
	{
		setState(e_stateCrossFade);
		getCrossFadeItem()->data()->reset();
		setFrameFadeTime(data->part(0).start());
		setAudioProcessType(1);
		emitOnCrossfade();
	}
}

//-------------------------------------------------------------------------------------------

bool AOBase::processCodecPlayPostProcess(AudioItem **pItem,const common::TimeStamp& currentT,bool completeFlag)
{
	bool loop = true;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPlayPostProcess - %.8f\n", static_cast<tfloat64>(currentT));
#endif

	if(getAudioProcessType()!=1)
	{
		if(completeFlag)
		{
			loop = processCodecPlayPostProcessComplete(pItem,currentT);
		}
		else
		{
			loop = processCodecPlayPostProcessRunning(pItem);
		}
	}
	return loop;
}

//-------------------------------------------------------------------------------------------

bool AOBase::processCodecPlayPostProcessComplete(AudioItem **pItem,const common::TimeStamp& currentT)
{
	bool loop;
	AudioItem *item = *pItem;
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPlayPostProcessComplete - %.8f\n", static_cast<tfloat64>(currentT));
#endif

	if(data->noParts() > 0)
	{
		if(getCodec()->isRemote())
		{
			loop = processCodecPlayPostProcessCompleteRemote(pItem,currentT);
		}
		else
		{
			loop = processCodecPlayPostProcessCompleteLocal(pItem);
		}
	}
	else
	{
		loop = false;
	}
	return loop;
}

//-------------------------------------------------------------------------------------------

bool AOBase::processCodecPlayPostProcessCompleteRemote(AudioItem **pItem,const common::TimeStamp& currentT)
{
	bool loop = true;
	AudioItem *item = *pItem;
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());
	common::TimeStamp endT = data->part(data->noParts() - 1).end();

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPlayPostProcessComplete - %.8f\n", static_cast<tfloat64>(currentT));
#endif

	if(getAudioStartFlag() && currentT>=endT)
	{
		*pItem = item->prev();
	}
	else
	{
		postProcess(data);
		item->setState(AudioItem::e_stateFull);
	}
	return loop;
}

//-------------------------------------------------------------------------------------------

bool AOBase::processCodecPlayPostProcessCompleteLocal(AudioItem **pItem)
{
	AudioItem *item = *pItem;
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPlayPostProcessCompleteLocal\n");
#endif	

	postProcess(data);
	item->setState(AudioItem::e_stateFull);
	return true;
}

//-------------------------------------------------------------------------------------------

bool AOBase::processCodecPlayPostProcessRunning(AudioItem **pItem)
{
	bool res = true;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPlayPostProcessRunning\n");
#endif

	if(getCodec()->isRemote() && !getCodec()->isComplete())
	{
		res = processCodecPlayPostProcessCheckBufferedState(pItem);
	}
	else
	{
		if(stopCodecDoNext())
		{
			processCodecPlayPostProcessRunningWithNext(pItem);
		}
		else
		{
			processCodecPlayPostProcessRunningWithNoNext(pItem);
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOBase::processCodecPlayPostProcessCheckBufferedState(AudioItem **pItem)
{
	tfloat64 limit;
	AudioItem *item = *pItem;
	common::TimeStamp limitT,currentT;
	bool res;
	
	currentT = timeFromEndOfItemBeingPlayed(item);
	
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPlayPostProcessCheckBufferedState - %.8f\n", static_cast<tfloat64>(currentT));
#endif
	
	limit = (static_cast<tfloat64>(getRemoteTimeSync()) * 3.0 ) / 10.0;
	limitT = limit;
	if(currentT < limitT)
	{
		setState(e_statePreBuffer);
		res = true;
	}
	else
	{
		res = false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void AOBase::processCodecPlayPostProcessRunningWithNext(AudioItem **pItem)
{
	AudioItem *item = *pItem;
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPlayPostProcessRunningWithNext\n");
#endif

	setAudioProcessType(2);
	setTrackTimeState(0);
	if(!data->rLength())
	{
		postProcess(data);
		item->setState(AudioItem::e_stateFull);
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::processCodecPlayPostProcessRunningWithNoNext(AudioItem **pItem)
{
	AudioItem *item = *pItem;
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());
	
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPlayPostProcessRunningWithNoNext\n");
#endif
	postProcess(data);
	item->setState(AudioItem::e_stateFullEnd);
}

//-------------------------------------------------------------------------------------------

bool AOBase::processCodecCrossFade(AudioItem* item,const common::TimeStamp&,bool& initF)
{
	tint i;
	bool flag = true,loop = true;
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());
	engine::RData *nextData;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecCrossFade\n");
#endif

	nextData = dynamic_cast<engine::RData *>(m_crossFadeItem->data());

	if(m_nextCodec!=0)
	{
		bool nextFlag = false;

		if(m_audioProcessType!=1)
		{
			flag = decodeAndResample(m_codec,item,initF);
			m_codecCurrentTime = data->part(data->noParts() - 1).end();
		}

		if(flag)
		{
			for(i=0;i<data->noParts();++i)
			{
				if(m_codecTimeLength<data->part(i).end())
				{
					if(m_codecTimeLengthUpdate)
					{
						m_codecTimeLength = data->part(i).end();
					}
					else
					{
						flag = false;
					}
				}
			}
		}
		else
		{
			if(m_codecTimeLengthUpdate)
			{
				m_codecTimeLength = data->part(data->noParts() - 1).end();
			}
			m_trackTimeState = 0;
		}

		if(m_nextCodec->isRemote())
		{
			tfloat32 percent = 0.0f;

			if(m_nextCodec->isBuffered(percent))
			{
				tint nState = 0;
				common::TimeStamp tE;

				if(m_frameFadeTime > m_crossFadeTime)
				{
					tE = m_frameFadeTime - m_crossFadeTime;
				}

				resetNextCrossData(nextData,tE);

				do
				{
					if(nextData->rLength() > 0)
					{
						nextFlag = decodeAndResample(m_nextCodec,m_crossFadeItem,initF);
					}
					else
					{
						nState = 2;
					}

					switch(nState)
					{
						case 0:
						if(nextFlag)
						{
							for(i=0;i<nextData->noParts();++i)
							{
								if(tE<nextData->part(i).start())
								{
									break;
								}
								else if(nextData->part(i).start()>=tE && tE<nextData->part(i).end())
								{
									break;
								}
							}

							if(i<nextData->noParts())
							{
								resetNextCrossData(nextData,tE);
								nState = 1;
							}
						}
						else
						{
							nextFlag = (!m_nextCodec->isComplete()) ? true : false;
						}
						break;

						case 1:
						if(!nextFlag)
						{
							nextFlag = (!m_nextCodec->isComplete()) ? true : false;
							nState = 2;
						}
						break;

						default:
							break;
					}
				} while(nState < 2);

				crossFade(data,nextData,m_frameFadeTime);
			}
			else
			{
				m_frameFadeTime = data->end();
				nextFlag = true;
			}
		}
		else
		{
			// Resample codec
			nextFlag = decodeAndResample(m_nextCodec,m_crossFadeItem,initF);
			crossFade(data,nextData,m_frameFadeTime);
			if(m_audioProcessType==1)
			{
				resetNextCrossData(nextData,m_frameFadeTime);
			}
			else
			{
				nextData->reset();
			}
			m_audioProcessType = 0;
		}

		if(flag)
		{
			if(nextFlag)
			{
				postProcess(data);
				item->setState(AudioItem::e_stateFull);
			}
			else
			{
				postProcess(data);
				item->setState(AudioItem::e_stateFull);
				m_state = e_statePlay;
				stopNextCodec();
			}
		}
		else
		{
			engine::RData::Part& p = data->part(data->noParts() - 1);
			p.start() = p.startNext();
			p.end() = p.endNext();
			p.setNext(true);
			p.refStartTime() = m_refStartAudioTime;
			m_refStartAudioTime = 0;

			if(nextFlag)
			{
				if(stopCodecDoNext())
				{
					postProcess(data);
					item->setState(AudioItem::e_stateFull);
				}
				else
				{
					postProcess(data);
					item->setState(AudioItem::e_stateFullEnd);
				}
			}
			else
			{
				postProcess(data);
				item->setState(AudioItem::e_stateFullEnd);
				m_state = e_stateNoCodec;
			}
		}
	}
	else
	{
		m_state = e_statePlay;
	}
	return loop;
}

//-------------------------------------------------------------------------------------------

bool AOBase::processCodecPreBuffer()
{
	bool loop = true;
	tfloat32 percent = 0.0f;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processCodecPreBuffer\n");
#endif

	if(m_codec->isBuffered(percent) || m_codec->isComplete())
	{
		if(m_codec->isComplete())
		{
			m_state = e_stateStop;
			emit onStop();
		}
		else
		{
			m_state = e_statePlay;
			emit onPlay();
		}
	}
	else
	{
		emit onBuffer(percent);
		loop = false;
	}
	return loop;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::timeFromEndOfItemBeingPlayed(AudioItem *item)
{
	common::TimeStamp totalT;
	common::TimeStamp playT = currentPlayTime();
	AudioItem *cItem = getCallbackAudioItem();

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::timeFromEndOfItemBeingPlayed\n");
#endif

	if(cItem!=0)
	{
		common::TimeStamp markT;
	
		if(cItem->state()!=AudioItem::e_stateEmpty)
		{
			bool endFlag = timeFromEndOfItemBeingPlayedHasData(cItem->next(),item);
			
			if(!(cItem->state()==AudioItem::e_stateDone && (cItem->next()->state()==AudioItem::e_stateDone || cItem->next()->state()==AudioItem::e_stateEmpty)))
			{
				tint i;
				bool isEnd = false;
				engine::RData *data = dynamic_cast<engine::RData *>(cItem->data());

				i = data->noParts() - 1;
				while(!isEnd && i>=0)
				{
					if(i==(data->noParts()-1))
					{
						totalT += timeFromEndOfItemBeingPlayedCallbackEndPart(cItem,endFlag,playT,isEnd,markT,item);
					}
					else
					{
						totalT += timeFromEndOfItemBeingPlayedCallbackPart(cItem,i,playT,isEnd,markT);
					}
					i--;
				}
			}
		}
		if(cItem != item)
		{
			AudioItem *pItem = cItem->next();

			while(pItem!=cItem && pItem->prev()!=item)
			{
				if(timeFromEndOfItemBeingPlayedHasData(pItem,item))
				{
					totalT += timeFromEndOfItemBeingPlayedItemTime(pItem,item);
				}
				pItem = pItem->next();
			}
		}
	}
	return totalT;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::timeFromEndOfItemBeingPlayedCallbackEndPart(AudioItem *item,
                                                                      bool endFlag,
                                                                      const common::TimeStamp& playT,
                                                                      bool& isEnd,
                                                                      common::TimeStamp& markT,
                                                                      AudioItem *targetItem)
{
	common::TimeStamp t,endT;
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());
	engine::RData::Part& p = data->part(data->noParts() - 1);

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::timeFromEndOfItemBeingPlayedCallbackEndPart\n");
#endif

	if(endFlag)
	{
		engine::RData *nData = dynamic_cast<engine::RData *>(item->next()->data());
		endT = nData->part(0).start();
	}
	else
	{
		endT = p.end();
	}
	
	if(endT <= playT)
	{
		if(p.isNext())
		{
			t = endT;
			if(data->noParts()==1)
			{
				isEnd = true;
			}
			else
			{
				markT = data->part(data->noParts()-2).end();
			}
		}
		else
		{
			t = 0.0;
			isEnd = true;
		}
	}
	else if(item==targetItem && endFlag && p.end() <= playT)
	{
		t = 0.0;
		isEnd = true;
	}
	else if(p.start() <= playT)
	{
        t = timeFromEndOfItemBeingPlayedDiff(endT,playT);
		isEnd = true;
	}
	else
	{
		if(p.isNext())
		{
            t = timeFromEndOfItemBeingPlayedDiff(endT,playT);
			isEnd = true;
		}
		else
		{
			if(data->noParts()==1)
			{
                t = timeFromEndOfItemBeingPlayedDiff(endT,playT);
				isEnd = true;
			}
			else
			{
                t = timeFromEndOfItemBeingPlayedDiff(endT,p.start());
				markT = p.start();
			}
		}
	}
	return t;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::timeFromEndOfItemBeingPlayedCallbackPart(AudioItem *item,
                                                                   int index,
                                                                   const common::TimeStamp& playT,
                                                                   bool& isEnd,
                                                                   common::TimeStamp& markT)
{
	common::TimeStamp t;
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());
	engine::RData::Part& p = data->part(index);

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::timeFromEndOfItemBeingPlayedCallbackPart\n");
#endif

	if(p.start() <= playT)
	{
        t = timeFromEndOfItemBeingPlayedDiff(markT,playT);
		isEnd = true;
	}
	else
	{
		if(p.isNext())
		{
			if(markT < playT)
			{
				t = markT;
				if(index==0)
				{
					isEnd = true;
				}
				else
				{
					markT = data->part(index-1).end();
				}
			}
			else
			{
                t = timeFromEndOfItemBeingPlayedDiff(markT,playT);
				isEnd = true;
			}		
		}
		else
		{
			if(index==0)
			{
                t = timeFromEndOfItemBeingPlayedDiff(markT,playT);
				isEnd = true;
			}
			else
			{
                t = timeFromEndOfItemBeingPlayedDiff(markT,playT);
				markT = p.start();
			}
		}
	}
	return t;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::timeFromEndOfItemBeingPlayedDiff(const common::TimeStamp& a,const common::TimeStamp& b)
{
    common::TimeStamp t;
    
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::timeFromEndOfItemBeingPlayedDiff\n");
#endif

    if(a > b)
    {
        t = a;
        t -= b;
    }
    return t;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::timeFromEndOfItemBeingPlayedItemTime(AudioItem *item,AudioItem *targetItem)
{
	tint i;
	common::TimeStamp t;
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::timeFromEndOfItemBeingPlayedItemTime\n");
#endif

	for(i=data->noParts()-1;i>=0;i--)
	{
		common::TimeStamp gapTime,partTime;
		engine::RData::Part& p = data->part(i);
	
		if(i==data->noParts()-1)
		{
			if(timeFromEndOfItemBeingPlayedHasData(item->next(),targetItem))
			{
				engine::RData *nData = dynamic_cast<engine::RData *>(item->next()->data());
				if(nData->noParts()>0 && p.end()<nData->part(0).start())
				{
					gapTime = nData->part(0).start() - p.end();
				}
			}
		}
		else if(p.end() < data->part(i+1).start())
		{
			gapTime = data->part(i+1).start() - p.end();
		}
		
		if(p.end() > p.start())
		{
			partTime = p.end() - p.start();
		}
		
		t += partTime + gapTime;
	}
	return t;
}

//-------------------------------------------------------------------------------------------
// Time is measured across the gap given that the next item:
//   1. Has data and thus time information - hence its state is set approriately.
//   2. Is in a state such that it has been filled in by processing a codec.
//   3. Is not the initial callback item
//   4. Is not the target item as measure finishes at end of its last part.
//-------------------------------------------------------------------------------------------

bool AOBase::timeFromEndOfItemBeingPlayedHasData(AudioItem *item,AudioItem *targetItem)
{
	bool res = false;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::timeFromEndOfItemBeingPlayedHasData\n");
#endif

	if(item!=getCallbackAudioItem() && item!=targetItem->next())
	{
		if(item->state()==AudioItem::e_stateFull || item->state()==AudioItem::e_stateFullEnd || item->state()==AudioItem::e_stateCodec ||
		   item->state()==AudioItem::e_stateCallback || item->state()==AudioItem::e_stateCallbackEnd)
		{
			res = true;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void AOBase::processComplete()
{
	int i;
	AudioItem *item = m_callbackAudioItem->prev();
	common::TimeStamp current(currentPlayTime());
	
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processComplete - %.8f\n", static_cast<tfloat64>(current));
#endif
	
	if(m_startNextTrackFlag)
	{
		emit onStart(m_nextName);
		m_startNextTrackFlag = false;
	}

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::onTime - %.8f\n", static_cast<tfloat64>(current));
#endif
	emit onTime(static_cast<quint64>(current));
	
	while(item!=m_callbackAudioItem && item->state()==AudioItem::e_stateDone)
	{
		if(m_codecState==e_codecCurrentFinish || m_codecState==e_codecSingleFinish)
		{
			if(m_completeCodec==0)
			{
				Q_ASSERT(false);
			}

			engine::RData *data = dynamic_cast<engine::RData *>(item->data());
			for(i=0;i<data->noParts();i++)
			{
				engine::RData::Part& part = data->part(i);
				if(part.isNext())
				{
					if(m_completeCodec!=0)
					{
						if(m_completeCodec->isRemote())
						{
							QObject::disconnect(m_completeCodec,SIGNAL(onInit(void*)),this,SLOT(onCodecInit(void*)));
						}
					}
					m_completeCodec->close();
					delete m_completeCodec;
					m_completeCodec = 0;

					if(m_codecState==e_codecCurrentFinish)
					{
						m_codecState = e_codecSingle;
					}
					else if(m_codecState==e_codecSingleFinish)
					{
						m_codecState = e_codecNoPlay;
					}
					break;
				}
			}
		}
		item->setState(AudioItem::e_stateEmpty);
		item->setDone(0);
		item->data()->reset();
		item = item->prev();
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::postProcess(engine::RData *)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::postProcess\n");
#endif
}

//-------------------------------------------------------------------------------------------

void AOBase::onTimer()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::onTimer\n");
#endif

	if(canCallSlot(e_onTimer))
	{
		doTimer();
		slotComplete();
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::doTimer()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::doTimer - %d\n", (int)m_state);
#endif

	processMessages();
	
	switch(m_state)
	{
		case e_statePreBuffer:
			processCodec();
			break;
			
		case e_statePlay:
		case e_stateCrossFade:
			processComplete();
			processCodec();
			break;
			
		case e_stateNoCodec:
			processComplete();
			break;
			
		case e_stateStop:
			stopCodec();
			break;
			
		case e_statePause:
			break;
	}
	
#if defined(DEBUG_LOG_AUDIOOUTPUT)
	printDebugLog();
#endif
}

//-------------------------------------------------------------------------------------------

void AOBase::onCodecInit(void *cPtr)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::onCodecInit\n");
#endif

	if(canCallSlot(e_onCodecInit,cPtr))
	{
        doCodecInit(cPtr);
		slotComplete();
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::doCodecInit(void *cPtr)
{
	engine::Codec *codec = reinterpret_cast<engine::Codec *>(cPtr);
	bool res = false;
	
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::onCodecInit\n");
#endif

	if(codec==m_codec)
	{
		if(m_codec->init())
		{
			m_frequency = m_codecFrequency = m_codec->frequency();
			m_noInChannels = m_codec->noChannels();
			if(m_noInChannels==2)
			{
				m_noOutChannels = 2;
			}
			else
			{
				m_noOutChannels = getNoChannelsMapped();
			}
			buildChannelMapArray();
			
			calcNextCodecTime();
			
			m_refStartAudioTime = getReferenceClockTime() + (1.0 - m_outputLatencyTime);
			m_state = e_statePlay;
			m_pauseTime = 0;

			if(openAudio())
			{
				m_currentOutTime = 0;
				m_audioStartFlag = false;
				m_mutexCount = 0;

				processCodec(true);
				
				if(startAudioDevice())
				{
					res = true;
				}
				else
				{
					printError("onCodecInit","Failed to start audio device playback");
				}
			}
			else
			{
				printError("onCodecInit","Failed to open audio device");
			}
		}
		else
		{
			printError("onCodecInit","Failed to initialize audio codec");
		}
		
		if(!res)
		{
			stopCodec();
		}
	}
	else if(codec==m_nextCodec)
	{
		if(m_nextCodec->init())
		{
			if(m_nextCodec->frequency()==m_frequency && m_nextCodec->noChannels()==m_noInChannels)
			{
				if(m_nrCrossfadeFlag && m_state==e_statePlay)
				{
					common::TimeStamp eC;
						
					m_crossFadeTime = m_codecCurrentTime + 0.01;
					eC = m_crossFadeTime + m_progFadeTime;
					if(eC < m_codecTimeLength)
					{
						m_codecTimeLength = eC;
					}
					m_codecTimeLengthUpdate = false;
					m_nextOutState = 1;
				}
				
				if(m_codecState==e_codecSingleFinish)
				{
					m_codecState = e_codecCurrentFinish;
					m_codec = m_nextCodec;
					m_nextCodec = 0;
					calcNextCodecTime();
					m_state = e_statePlay;
				}
				else
				{
					m_codecState = e_codecNextQueued;
				}
				res = true;
			}
		}
		else
		{
			common::BString err("Failed to initialized codec for '");
			err += m_nextName.toUtf8().constData();
			err += "'";
			printError("onCodecInit",static_cast<const tchar *>(err));					
		}
		
		if(!res)
		{
			stopNextCodec();
			if(!m_nextOutState)
			{
				m_nextOutState = 2;
				emit onNoNext();
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

bool AOBase::pausePlayback(bool shutdown,bool signalFlag)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::pausePlayback\n");
#endif

	if(getState()!=e_statePause && getState()!=e_stateStop)
	{
		setPauseTime(currentPlayTime());
		setPauseAudioFlag(true);
		setState(e_statePause);
		
		stopAudioDevice();
		if(shutdown)
		{
			closeAudio();
		}
		if(signalFlag)
		{
			emitOnPause();
		}

		resetLFEChannel();
	}
	return true;
}

//-------------------------------------------------------------------------------------------

bool AOBase::unpausePlayback(bool signalFlag)
{
	bool res = false;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::unpausePlayback\n");
#endif

	if(getState()==e_statePause)
	{
		bool process = true;
		
		flushCyclicBuffer();
		
		switch(getCodecState())
		{
			case e_codecNoPlay:
				process = unpausePlaybackCodecStateNoPlay();
				break;

			case e_codecSingle:
			case e_codecNextQueued:
				process = unpausePlaybackCodecStateSingle();
				break;

			case e_codecSingleFinish:
			case e_codecCurrentFinish:
				res = unpausePlaybackCodecStateFinish();
				process = false;
				break;
		}
		
		if(process)
		{
			res = unpausePlaybackProcess(signalFlag);
		}
	}
	else
	{
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOBase::unpausePlaybackCodecStateNoPlay()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::unpausePlaybackCodecStateNoPlay\n");
#endif
	m_state = e_stateStop;
	return false;
}

//-------------------------------------------------------------------------------------------

bool AOBase::unpausePlaybackCodecStateSingle()
{
	bool process = unpausePlaybackCodecStateSingleTiming();
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::unpausePlaybackCodecStateSingle\n");
#endif
	unpausePlaybackCodecStateSinglePlayState(process);
	return process;
}

//-------------------------------------------------------------------------------------------

bool AOBase::unpausePlaybackCodecStateSingleTiming()
{
	bool process = unpausePlaybackCodecStateSingleSeekToPauseTime();
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::unpausePlaybackCodecStateSingleTiming\n");
#endif
	return unpausePlaybackCodecStateSingleBufferOnRemote(process);
}

//-------------------------------------------------------------------------------------------

bool AOBase::unpausePlaybackCodecStateSingleSeekToPauseTime()
{
	bool process = true;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::unpausePlaybackCodecStateSingleSeekToPauseTime\n");
#endif

	if(getCodec()->isSeek())
	{
		if(!getCodec()->seek(getPauseTime()))
		{
			stopCodec(true);
			process = false;
		}
	}
	return process;
}

//-------------------------------------------------------------------------------------------

bool AOBase::unpausePlaybackCodecStateSingleBufferOnRemote(bool process)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::unpausePlaybackCodecStateSingleBufferOnRemote\n");
#endif

	if(process && getCodec()->isRemote())
	{
		common::TimeStamp bT = getRemoteTimeSync();
		getCodec()->buffer(bT);
		setState(e_statePreBuffer);
		process = false;
	}
	return process;
}

//-------------------------------------------------------------------------------------------

void AOBase::unpausePlaybackCodecStateSinglePlayState(bool process)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::unpausePlaybackCodecStateSinglePlayState\n");
#endif

	if(process && getNextCodec()!=0 && getCrossFadeFlag() && getPauseTime()>=getCrossFadeTime())
	{
		unpausePlaybackCodecStateSinglePlayToCrossfadeState();
	}
	else
	{
		unpausePlaybackCodecStateSingleSetPlayState(process);
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::unpausePlaybackCodecStateSinglePlayToCrossfadeState()
{
	common::TimeStamp bT;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::unpausePlaybackCodecStateSinglePlayToCrossfadeState\n");
#endif

	if(getNextCodec()->isSeek())
	{
		bT = getPauseTime() - getCrossFadeTime();
		if(!getNextCodec()->seek(bT))
		{
			stopNextCodec();
		}
	}
	if(getNextCodec()!=0)
	{
		if(getNextCodec()->isRemote())
		{
			bT = getRemoteTimeSync();
			getNextCodec()->buffer(bT);
		}
		setNextOutState(1);
		setState(e_stateCrossFade);
	}
	else
	{
		unpausePlaybackCodecStateSingleSetPlayState(true);
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::unpausePlaybackCodecStateSingleSetPlayState(bool process)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::unpausePlaybackCodecStateSingleSetPlayState\n");
#endif

	setNextOutState(0);
	if(process)
	{
		setState(e_statePlay);
	}
}

//-------------------------------------------------------------------------------------------

bool AOBase::unpausePlaybackCodecStateFinish()
{
	bool res;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::unpausePlaybackCodecStateFinish\n");
#endif

	if(getCompleteCodec()->isSeek())
	{
		setNextCodec(getCodec());
		setCodec(getCompleteCodec());
		setCompleteCodec(0);
		setCodecState((getNextCodec()!=0) ? e_codecNextQueued : e_codecSingle);
		setTrackTimeStateFlag(true);
		if(getNextCodec()!=0)
		{
			if(getNextCodec()->isSeek())
			{
				common::TimeStamp t(0);
				if(!getNextCodec()->seek(t))
				{
					stopNextCodec();
				}
			}
		}
		calcNextCodecTime();
		res = unpausePlayback(true);
	}
	else
	{
		stopCodec(true);
		res = false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOBase::unpausePlaybackProcess(bool signalFlag)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::unpausePlaybackProcess\n");
#endif
	unpausePlaybackProcessSetTimeAndState();
	unpausePlaybackProcessOpenAudio();
	return unpausePlaybackProcessRestartPlayback(signalFlag);
}

//-------------------------------------------------------------------------------------------

void AOBase::unpausePlaybackProcessSetTimeAndState()
{
	common::TimeStamp pTime = getPauseTime();
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::unpausePlaybackProcessSetTimeAndState\n");
#endif
	setCurrentOutTime(pTime);
	setCurrentPlayTime(pTime);
	setCurrentCallbackTime(pTime);
	setPauseAudioFlag(false);
	setAudioStartFlag(false);
}

//-------------------------------------------------------------------------------------------

void AOBase::unpausePlaybackProcessOpenAudio()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::unpausePlaybackProcessOpenAudio\n");
#endif
	resetResampler();
	if(!isAudio())
	{
		if(!openAudio())
		{
			printError("unpausePlayback","Error opening audio device");
			closeAudio();
		}
	}
}

//-------------------------------------------------------------------------------------------

bool AOBase::unpausePlaybackProcessRestartPlayback(bool signalFlag)
{
	bool res = false;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::unpausePlaybackProcessRestartPlayback\n");
#endif

	if(isAudio())
	{
		processCodec(false);
		
		if(startAudioDevice())
		{
			res = true;
			if(signalFlag)
			{
				emitOnPlay();
			}
		}
		else
		{
			printError("unpausePlayback","Error starting audio device");
			stopCodec(true);
		}
	}
	else
	{
		stopCodec(true);
    }
	return res;
}

//-------------------------------------------------------------------------------------------

void AOBase::reloadChannelSettings()
{
	m_deviceInfoMutex.lock();
	if(m_deviceInfo != 0 && m_deviceIdx >= 0 && m_deviceIdx < m_deviceInfo->noDevices())
	{
		AOQueryDevice::Device *dev = m_deviceInfo->deviceDirect(m_deviceIdx);
		dev->loadChannelMap();
	}
	m_deviceInfoMutex.unlock();
}

//-------------------------------------------------------------------------------------------

bool AOBase::resetPlayback()
{
	bool res = false;
	
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::resetPlayback\n");
#endif
	
	reloadChannelSettings();
	
	if(m_state!=e_statePause && m_state!=e_stateStop)
	{
		m_pauseTime = currentPlayTime();
		m_pauseAudioFlag = true;
		m_state = e_statePause;
		
		if(isAudio())
		{
			common::TimeStamp bT;
			bool process = true;
			
			stopAudioDevice();
			closeAudio();
			flushCyclicBuffer();
			
			switch(m_codecState)
			{
				case e_codecNoPlay:
					m_state = e_stateStop;
					process = false;
					break;

				case e_codecSingleFinish:
				case e_codecCurrentFinish:
					{
						if(m_completeCodec->isSeek())
						{
							m_nextCodec = m_codec;
							m_codec = m_completeCodec;
							m_completeCodec = 0;
							m_codecState = (m_nextCodec!=0) ? e_codecNextQueued : e_codecSingle;
							m_trackTimeStateFlag = true;
							if(m_nextCodec!=0)
							{
								if(m_nextCodec->isSeek())
								{
									common::TimeStamp t(0);
									if(!m_nextCodec->seek(t))
									{
										stopNextCodec();
									}
								}
							}
							calcNextCodecTime();
						}
						else
						{
							stopCodec();
							process = false;
							res = false;
							break;
						}
					}

				case e_codecSingle:
				case e_codecNextQueued:
					{
						if(m_codec->isSeek())
						{
							if(!m_codec->seek(m_pauseTime))
							{
								stopCodec();
								process = false;
							}
						}
						if(process && m_codec->isRemote())
						{
							bT = getRemoteTimeSync();
							m_codec->buffer(bT);
							m_state = e_statePreBuffer;
							process = false;
						}
		
						if(m_nextCodec!=0 && m_crossFadeFlag && m_pauseTime>=m_crossFadeTime)
						{
							if(m_nextCodec->isSeek())
							{
								bT = m_pauseTime - m_crossFadeTime;
								if(!m_nextCodec->seek(bT))
								{
									stopNextCodec();
								}
							}
							if(m_nextCodec->isRemote())
							{
								bT = getRemoteTimeSync();
								m_nextCodec->buffer(bT);
							}
							m_nextOutState = 1;
							m_state = e_stateCrossFade;
						}
						else
						{
							m_nextOutState = 0;
							m_state = e_statePlay;
						}
					}
					break;
			}
			
			if(process)
			{
				m_currentOutTime = m_pauseTime;
				m_pauseAudioFlag = false;
				m_audioStartFlag = false;
				
				if(m_codec!=0)
				{
					m_frequency = m_codecFrequency = m_codec->frequency();
					m_noInChannels = m_codec->noChannels();
					if(m_noInChannels!=2)
					{
						m_noOutChannels = getNoChannelsMapped();
					}
					else
					{
						m_noOutChannels = 2;
					}
					buildChannelMapArray();
				}

				if(openAudio())
				{
					processCodec();
					
					if(startAudioDevice())
					{
						res = true;
					}
					else
					{
						printError("resetPlayback","Error starting audio device");
					}
				}
				else
				{
					printError("resetPlayback","Error opening audio device");
				}
			}
		}
	}
	else
	{
		if(isAudio())
		{
			closeAudio();
		}
		res = true;
	}
	
	if(!res)
	{
		stopCodec();
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOBase::seekPlayback(const common::TimeStamp& t)
{
	bool res = false;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::seekPlayback - %.8f\n", static_cast<tfloat64>(t));
#endif

	if(m_state!=e_statePause)
	{
		if(pausePlayback(false,false))
		{
			m_pauseTime = t;
			if(unpausePlayback(false))
			{
				res = true;
			}
			else
			{
				printError("seekPlayback","Failed to start audio playback from requested position");
			}
		}
		else
		{
			printError("seekPlayback","Failed to pause audio");
		}
	}
	else
	{
		m_pauseTime = t;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::getRemoteTimeSync()
{
	common::TimeStamp tS(1.0);
	return tS;
}

//-------------------------------------------------------------------------------------------

void AOBase::setDeviceID(tint idIndex)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::setDeviceID - %d\n", idIndex);
#endif

	if(idIndex>=0 && idIndex<m_deviceInfo->noDevices())
	{
		QSettings settings;
		settings.beginGroup("audio");
		settings.setValue(QString::fromLatin1("defaultDeviceID"),QVariant(m_deviceInfo->device(idIndex).idConst()));
		settings.endGroup();
		m_deviceIdx = idIndex;
		m_deviceInfoMutex.lock();
		m_deviceInfo->deviceDirect(idIndex)->loadChannelMap();
		m_deviceInfoMutex.unlock();
		resetPlayback();
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::doUpdateChannelMap(tint devId)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::setChannelMap\n");
#endif
	m_deviceInfoMutex.lock();
	if(devId >= 0 && devId < m_deviceInfo->noDevices())
	{
		m_deviceInfo->deviceDirect(devId)->loadChannelMap();
	}
	m_deviceInfoMutex.unlock();
	
	if(devId == m_deviceIdx)
	{
		resetPlayback();
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::doSetVolume(sample_t vol, bool isCallback)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::doSetVolume\n");
#endif

	if(vol<c_zeroSample)
	{
		m_volume = c_zeroSample;
	}
	else if(vol>c_plusOneSample)
	{
		m_volume = c_plusOneSample;
	}
	else
	{
		m_volume = vol;
	}
	if(isCallback)
	{
		emitOnVolumeChanged(m_volume);
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::doSetCrossFade(const common::TimeStamp& t)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::doSetCrossFade\n");
#endif

	m_progFadeState = 1;
	m_progFadeTime = t;
}

//-------------------------------------------------------------------------------------------

void AOBase::calcNextCodecTime()
{
	m_nextOutState = 0;
	m_pauseAudioFlag = false;
	m_trackTimeState = 0;
	m_codecTimeLength = m_codec->length();
	m_codecTimeLengthUpdate = true;
	
	calcCrossFadeTime();
	if(m_crossFadeTime >= 1.0)
	{
		if(m_codecTimeLength>5.0)
		{
			m_nextCodecTime = m_crossFadeTime - 5.0;
		}
		else
		{
			m_nextCodecTime = m_crossFadeTime;
		}
	}
	else
	{
		m_nextCodecTime = 0.0;
	}
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::calcNextCodecTime - %.8f\n", static_cast<tfloat64>(m_nextCodecTime));
#endif
}

//-------------------------------------------------------------------------------------------

void AOBase::calcCrossFadeTime()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::calcCrossFadeTime\n");
#endif
	calcCrossFadeTimeSetLength();
	calcCrossFadeTimeAdjustToCodecLength();
}

//-------------------------------------------------------------------------------------------

void AOBase::calcCrossFadeTimeSetLength()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::calcCrossFadeTimeSetLength\n");
#endif

	if(getProgFadeState())
	{
		setCrossFadeTimeLen(getProgFadeTime());
	}
	setProgFadeState(0);

	setCrossFadeTime(getCrossFadeTimeLen());
	setCrossFadeFlag((getCrossFadeTime()!=0) ? true : false);
}

//-------------------------------------------------------------------------------------------

void AOBase::calcCrossFadeTimeAdjustToCodecLength()
{
	common::TimeStamp t;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::calcCrossFadeTimeAdjustToCodecLength\n");
#endif
	if(getCrossFadeFlag() && getCrossFadeTime() < getCodecTimeLengthAdjustedForEndPosition())
	{
		t = getCodecTimeLengthAdjustedForEndPosition() - getCrossFadeTime();
	}
	else
	{
		t = getCodecTimeLengthAdjustedForEndPosition();
	}
	setCrossFadeTime(t);
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::getCodecTimeLengthAdjustedForEndPosition()
{
	return (getCodecTimePositionComplete() > 0) ? getCodecTimePositionComplete() : getCodecTimeLength();
}

//-------------------------------------------------------------------------------------------

void AOBase::initCrossFadeWindow()
{
	tint i;
	tfloat64 fac = c_PI_D / 4096.0;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::initCrossFadeWindow\n");
#endif

	m_crossFadeAWin = new sample_t [2048 + 10];
	m_crossFadeBWin = new sample_t [2048 + 10];
	
	for(i=0;i<2048;++i)
	{
		m_crossFadeAWin[i] = static_cast<sample_t>(::cos(static_cast<tfloat64>(i) * fac) * ::cos(static_cast<tfloat64>(i) * fac));
		m_crossFadeBWin[i] = static_cast<sample_t>(::sin(static_cast<tfloat64>(i) * fac) * ::sin(static_cast<tfloat64>(i) * fac));
	}
	for(i=2048;i<2058;++i)
	{
		m_crossFadeAWin[i] = c_zeroSample;
		m_crossFadeBWin[i] = c_plusOneSample;
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::crossFade(engine::RData *dataA,engine::RData *dataB,common::TimeStamp& tStart)
{
	tint i,j,k,l,total;
	tint pA,pB;
	common::TimeStamp tCurrent,tSample,tEnd,tNext;
	sample_t *inA,*inB,*out;
	tfloat64 fLen,freq = static_cast<sample_t>(m_frequency);

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::crossFade\n");
#endif

	total = dataA->length();
	fLen = static_cast<tfloat64>(m_codecTimeLength - m_crossFadeTime);
	
	tSample = 1.0 / freq;
	tEnd += tStart + (static_cast<tfloat64>(total) / freq);

	l = 0;
	pA = 0;
	inA = 0;
	tNext = dataA->part(0).start();
	for(i=0,k=0,tCurrent=tStart;i<total;++i,tCurrent+=tSample)
	{
		if(tCurrent>=tNext)
		{
			while(pA<dataA->noParts())
			{
				if(tCurrent>=dataA->part(pA).start())
				{
					if(tCurrent<dataA->part(pA).end())
					{
						tNext = dataA->part(pA).end();
						inA = dataA->partDataOut(pA);
						l = static_cast<tint>(static_cast<tfloat64>(tCurrent - dataA->part(pA).start()) * freq) * m_noOutChannels;
						break;
					}
					else
					{
						pA++;
					}
				}
				else
				{
					tNext = dataA->part(pA).start();
					inA = 0;
					break;
				}
			}
			if(pA>=dataA->noParts())
			{
				inA = 0;
			}
		}
		
		for(j=0;j<m_noOutChannels;++j,++k)
		{
			if(inA!=0)
			{
				m_crossASample[k] = inA[l++];
			}
			else
			{
				m_crossASample[k] = c_zeroSample;
			}
		}
	}
	
	pB = 0;
	inB = 0;
	if(tStart < m_crossFadeTime)
	{
		tNext = m_crossFadeTime;
	}
	else
	{
		tNext = m_crossFadeTime + dataB->part(0).start();
	}
	for(i=0,k=0,tCurrent=tStart;i<total;++i,tCurrent+=tSample)
	{
		if(tCurrent>=tNext)
		{
			while(pB<dataB->noParts())
			{
				common::TimeStamp pStart,pEnd;
				
				pStart = m_crossFadeTime + dataB->part(pB).start();
				pEnd = m_crossFadeTime + dataB->part(pB).end();
				
				if(tCurrent>=pStart)
				{
					if(tCurrent<pEnd)
					{
						tNext = pEnd;
						inB = dataB->partDataOut(pB);
						l = static_cast<tint>(static_cast<tfloat64>(tCurrent - pStart) * freq) * m_noOutChannels;
						break;
					}
					else
					{
						pB++;
					}
				}
				else
				{
					tNext = pStart;
					inB = 0;
					break;
				}
			}
			if(pB>=dataB->noParts())
			{
				inB = 0;
			}
		}
		
		for(j=0;j<m_noOutChannels;++j,++k)
		{
			if(inB!=0)
			{
				m_crossBSample[k] = inB[l++];
			}
			else
			{
				m_crossBSample[k] = c_zeroSample;
			}
		}
	}
	
	dataA->reset();
	
	i = 0;
	k = 0;
	tCurrent = tStart;
	dataA->start() = tStart;
	
	if(tCurrent<m_crossFadeTime)
	{
		engine::RData::Part& part = dataA->nextPart();
		
		part.start() = tStart;
		out = dataA->partDataOut(dataA->noParts() - 1);
		total = dataA->rLength() * m_noOutChannels;
		
		while(tCurrent<m_crossFadeTime && i<total)
		{
			for(j=0;j<m_noOutChannels;++j,++k)
			{
				out[i + j] = m_crossASample[k];
			}
			tCurrent += tSample;
			i += m_noOutChannels;
		}
		
		part.end() = tCurrent;
		part.length() = i / m_noOutChannels;
		part.done() = true;
	}
	
	if(tCurrent>=m_crossFadeTime && tCurrent<m_codecTimeLength)
	{
		engine::RData::Part& part = dataA->nextPart();
		
		part.start() = tCurrent;
		part.startNext() = tCurrent - m_crossFadeTime;
		out = dataA->partDataOut(dataA->noParts() - 1);
		i = 0;
		total = dataA->rLength() * m_noOutChannels;
		
		while(tCurrent<m_codecTimeLength && i<total)
		{
			tint pos = static_cast<tint>((static_cast<tfloat64>(tCurrent - m_crossFadeTime) / fLen) * 2048.0);
			
			for(j=0;j<m_noOutChannels;++j,++k)
			{
				out[i + j] = (m_crossASample[k] * m_crossFadeAWin[pos]) + (m_crossBSample[k] * m_crossFadeBWin[pos]);
			}
			tCurrent += tSample;
			i += m_noOutChannels;
		}
		
		part.end() = tCurrent;
		part.endNext() = tCurrent - m_crossFadeTime;
		part.length() = i / m_noOutChannels;
		part.done() = true;
	}
	
	if(tCurrent>=m_codecTimeLength)
	{
		engine::RData::Part& part = dataA->nextPart();
		
		part.start() = tCurrent;
		part.startNext() = tCurrent - m_crossFadeTime;

		out = dataA->partDataOut(dataA->noParts() - 1);
		i = 0;
		total = dataA->rLength() * m_noOutChannels;
		
		while(i<total)
		{
			for(j=0;j<m_noOutChannels;++j,++k)
			{
				out[i + j] = m_crossBSample[k];
			}
			tCurrent += tSample;
			i += m_noOutChannels;
		}
		
		part.end() = tCurrent;
		part.endNext() = tCurrent - m_crossFadeTime;
		part.length() = i / m_noOutChannels;
		part.done() = true;
	}
	
	dataA->end() = tEnd;
	tStart = tEnd;
}

//-------------------------------------------------------------------------------------------

void AOBase::resetNextCrossData(engine::RData *fromD,const common::TimeStamp& tEnd)
{
	tint i,k=0,offset,len;
	common::TimeStamp tE;
	QVector<engine::RData::Part> parts;
	QVector<engine::RData::Part>::iterator ppI;
	sample_t *inD,*outD;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::resetNextCrossData\n");
#endif

	tE = tEnd - m_crossFadeTime;
	
	for(i=0;i<fromD->noParts();++i)
	{
		engine::RData::Part& pt = fromD->part(i);

		if(tE < pt.start())
		{
			break;
		}
		else if(pt.start()<=tE && tE<pt.end())
		{
			break;
		}
	}
	
	if(i<fromD->noParts())
	{
		tfloat64 freq = static_cast<tfloat64>(m_frequency);
		
		outD = fromD->data();
		
		if(fromD->part(i).start()<=tE && tE<fromD->part(i).end())
		{
			offset = static_cast<tint>(static_cast<tfloat64>(tE - fromD->part(i).start()) * freq);
			len = fromD->part(i).length();
			if(offset < len)
			{
				engine::RData::Part nP;

				nP.start() = tE;
				nP.end() = fromD->part(i).end();
				nP.length() = len - offset;
				nP.done() = true;
				
				offset *= m_noOutChannels;
				len *= m_noOutChannels;
				
				inD = fromD->partDataOut(i);
				while(offset < len)
				{
					outD[k++] = inD[offset++];
				}
				
				parts.append(nP);
			}
			i++;
		}
		
		while(i<fromD->noParts())
		{
			engine::RData::Part nP;
			engine::RData::Part& cP = fromD->part(i);
			
			nP.start() = cP.start();
			nP.end() = cP.end();
			nP.length() = cP.length();
			nP.done() = true;
			parts.append(nP);
			
			offset = 0;			
			len = cP.length() * m_noOutChannels;
			
			inD = fromD->partDataOut(i);
			while(offset < len)
			{
				outD[k++] = inD[offset++];
			}
			
			i++;			
		}
		
		fromD->reset();
		
		for(ppI=parts.begin();ppI!=parts.end();++ppI)
		{
			engine::RData::Part& p = fromD->nextPart();
			engine::RData::Part& cP = *ppI;
			
			p.length() = cP.length();
			p.start() = cP.start();
			p.end() = cP.end();
			p.done() = true;
		}
		fromD->start() = fromD->part(0).start();
		fromD->end() = fromD->part(fromD->noParts() - 1).end();
	}
	else
	{
		fromD->reset();
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::postAudioEvent(QEvent *e)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::postAudioEvent\n");
#endif

	if(e!=0 && e->type()>=static_cast<QEvent::Type>(AudioEvent::e_startPlaybackEvent))
	{
		m_eventQueueMutex.lock();
		m_eventQueue.prepend(e);
		m_eventQueueMutex.unlock();
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::onEventTimer()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::onEventTimer\n");
#endif

	if(canCallSlot(e_onEventTimer))
	{
		doEventTimer();
		slotComplete();
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::logAudioEvent(const tchar *strR,AudioEvent *audioE)
{
	QString typeStr;
	AudioEvent::AudioEventType type = static_cast<AudioEvent::AudioEventType>(audioE->type());
	
	switch(type)
	{
		case AudioEvent::e_startPlaybackEvent:
			typeStr = "e_startPlaybackEvent"; break;
		case AudioEvent::e_stopPlaybackEvent:
			typeStr = "e_stopPlaybackEvent"; break;
		case AudioEvent::e_playPlaybackEvent:
			typeStr = "e_playPlaybackEvent"; break;
		case AudioEvent::e_pausePlaybackEvent:
			typeStr = "e_pausePlaybackEvent"; break;
		case AudioEvent::e_seekPlaybackEvent:
			typeStr = "e_seekPlaybackEvent"; break;
		case AudioEvent::e_setVolumeEvent:
			typeStr = "e_setVolumeEvent"; break;
		case AudioEvent::e_setDeviceID:
			typeStr = "e_setDeviceID"; break;
		case AudioEvent::e_updateChannelMap:
			typeStr = "e_updateChannelMap"; break;
		case AudioEvent::e_nextPlaybackEvent:
			typeStr = "e_nextPlaybackEvent"; break;
		case AudioEvent::e_nextCrossPlaybackEvent:
			typeStr = "e_nextCrossPlaybackEvent"; break;
		case AudioEvent::e_crossFadeEvent:
			typeStr = "e_crossFadeEvent"; break;
		case AudioEvent::e_audioDeviceChangeEvent:
			typeStr = "e_audioDeviceChangeEvent"; break;
        case AudioEvent::e_resetPlaybackEvent:
            typeStr = "e_resetPlaybackEvent"; break;
	}
	common::Log::g_Log.print("AOBase::%s : AudioEvent - %s, %s, %.8f, %.8f\n",strR,typeStr.toUtf8().constData(),audioE->uri().toUtf8().constData(),static_cast<tfloat64>(audioE->time()),static_cast<tfloat64>(audioE->timeLength()));
}

//-------------------------------------------------------------------------------------------

void AOBase::doEventTimer()
{
	QEvent *e;
	bool loop = true;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::doEventTimer\n");
#endif

	while(loop)
	{
		AudioEvent *audioE = 0;
		
		m_eventQueueMutex.lock();
		if(!m_eventQueue.isEmpty())
		{
			e = m_eventQueue.takeLast();
			
			audioE = dynamic_cast<AudioEvent *>(e);
			if(audioE!=0)
			{
				int i;
				bool ignore = false;
				AudioEvent::AudioEventType type = static_cast<AudioEvent::AudioEventType>(e->type());

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
				logAudioEvent("doEventTimer",audioE);
#endif

				if(type==AudioEvent::e_startPlaybackEvent || type==AudioEvent::e_stopPlaybackEvent || type==AudioEvent::e_nextCrossPlaybackEvent || type==AudioEvent::e_nextPlaybackEvent)
				{
					for(i=m_eventQueue.size()-1;i>=0 && !ignore;i--)
					{
						AudioEvent *aE = dynamic_cast<AudioEvent *>(m_eventQueue.at(i));
						if(aE!=0)
						{
							AudioEvent::AudioEventType aType = static_cast<AudioEvent::AudioEventType>(aE->type());
							switch(aType)
							{
								case AudioEvent::e_startPlaybackEvent:
								case AudioEvent::e_stopPlaybackEvent:
								case AudioEvent::e_nextCrossPlaybackEvent:
									ignore = true;
									break;
								
								default:
									break;
							}
						}
					}
				}
				else if(type==AudioEvent::e_playPlaybackEvent || type==AudioEvent::e_pausePlaybackEvent)
				{
					for(i=m_eventQueue.size()-1;i>=0 && !ignore;i--)
					{
						AudioEvent *aE = dynamic_cast<AudioEvent *>(m_eventQueue.at(i));
						if(aE!=0)
						{
							AudioEvent::AudioEventType aType = static_cast<AudioEvent::AudioEventType>(aE->type());
							switch(aType)
							{
								case AudioEvent::e_startPlaybackEvent:
								case AudioEvent::e_stopPlaybackEvent:
								case AudioEvent::e_playPlaybackEvent:
								case AudioEvent::e_pausePlaybackEvent:
								case AudioEvent::e_nextCrossPlaybackEvent:
									ignore = true;
									break;
									
								default:
									break;
							}
						}
					}					
				}
				if(ignore)
				{
					delete audioE;
					audioE = 0;
				}
			}
			else
			{
				if(e!=0)
				{
					delete e;
				}
			}
		}
		else
		{
			loop = false;
		}
		m_eventQueueMutex.unlock();
		
		if(audioE!=0)
		{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
			common::Log::g_Log.print("AOBase::doEventTimer - AudioEvent - Process\n");
#endif
			audioEventMain(audioE);
			delete audioE;
		}
	}
}

//-------------------------------------------------------------------------------------------

bool AOBase::event(QEvent *e)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::event()\n");
#endif

	if(e->type()>=static_cast<QEvent::Type>(AudioEvent::e_startPlaybackEvent))
	{
		AudioEvent *audioE = dynamic_cast<AudioEvent *>(e);
		
		if(audioE!=0)
		{
			audioE->setAccepted(true);
			
			AudioEvent *audioEventClone = AudioEvent::clone(audioE);
			if(canCallSlot(e_onAudioEvent,reinterpret_cast<void *>(audioEventClone)))
			{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
				logAudioEvent("event",audioE);
				common::Log::g_Log.print("AOBase::event - AudioEvent - Process\n");
#endif
				audioEventMain(audioEventClone);
				slotComplete();
			}
			
			return true;
		}
		else
		{
			return QObject::event(e);
		}
	}
	else
	{
		return QObject::event(e);
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::audioEventMain(AudioEvent *e)
{
	switch(m_state)
	{
		case e_stateNoCodec:
			audioEventNoCodecState(e);
			break;
			
		case e_statePreBuffer:
			audioEventPreBufferState(e);
			break;
			
		case e_statePlay:
			audioEventPlayState(e);
			break;
			
		case e_stateCrossFade:
			audioEventCrossFadeState(e);
			break;
			
		case e_statePause:
			audioEventPauseState(e);
			break;
			
		case e_stateStop:
			audioEventStopState(e);
			break;
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::audioEventStopState(AudioEvent *e)
{
	AudioEvent::AudioEventType type = static_cast<AudioEvent::AudioEventType>(e->type());

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::audioEventStopState()\n");
#endif
	
	switch(type)
	{
		case AudioEvent::e_startPlaybackEvent:
			startCodec(e->uri(),e->time(),e->timeLength());
			break;
			
		case AudioEvent::e_stopPlaybackEvent:
			break;
			
		case AudioEvent::e_playPlaybackEvent:
			break;
			
		case AudioEvent::e_pausePlaybackEvent:
			break;
			
		case AudioEvent::e_seekPlaybackEvent:
			break;
			
		case AudioEvent::e_setVolumeEvent:
			doSetVolume(e->volume(), e->isCallback());
			break;
						
		case AudioEvent::e_setDeviceID:
			setDeviceID(e->device());
			break;
			
		case AudioEvent::e_updateChannelMap:
			doUpdateChannelMap(e->device());
			break;
			
		case AudioEvent::e_nextPlaybackEvent:
			startCodec(e->uri(),e->time(),e->timeLength());
			break;
			
		case AudioEvent::e_nextCrossPlaybackEvent:
			startCodec(e->uri(),e->time(),e->timeLength());
			break;
			
		case AudioEvent::e_crossFadeEvent:
			doSetCrossFade(e->time());
			break;
			
		case AudioEvent::e_audioDeviceChangeEvent:
			audioDeviceChange();
			break;
			
		case AudioEvent::e_resetPlaybackEvent:
			break;
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::audioEventPlayState(AudioEvent *e)
{
	AudioEvent::AudioEventType type = static_cast<AudioEvent::AudioEventType>(e->type());

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::audioEventPlayState()\n");
#endif

	switch(type)
	{
		case AudioEvent::e_startPlaybackEvent:
			stopCodec(false);
			startCodec(e->uri(),e->time(),e->timeLength());
			break;
			
		case AudioEvent::e_stopPlaybackEvent:
			stopCodec();
			break;
			
		case AudioEvent::e_playPlaybackEvent:
			break;
			
		case AudioEvent::e_pausePlaybackEvent:
			pausePlayback(true);
			break;
			
		case AudioEvent::e_seekPlaybackEvent:
			seekPlayback(e->time());
			break;
			
		case AudioEvent::e_setVolumeEvent:
			doSetVolume(e->volume(), e->isCallback());
			break;
						
		case AudioEvent::e_setDeviceID:
			setDeviceID(e->device());
			break;

		case AudioEvent::e_updateChannelMap:
			doUpdateChannelMap(e->device());
			break;
			
		case AudioEvent::e_nextPlaybackEvent:
			startNextCodec(e->uri(),e->time(),e->timeLength());
			break;
		
		case AudioEvent::e_nextCrossPlaybackEvent:
			if(m_crossFadeFlag)
			{
				startNextCodec(e->uri(),e->time(),e->timeLength(),true);
			}
			else
			{
				stopCodec(false);
				startCodec(e->uri(),e->time(),e->timeLength());
			}
			break;
			
		case AudioEvent::e_crossFadeEvent:
			doSetCrossFade(e->time());
			break;

		case AudioEvent::e_audioDeviceChangeEvent:
			audioDeviceChange();
			break;
			
		case AudioEvent::e_resetPlaybackEvent:
			resetPlayback();
			break;
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::audioEventPauseState(AudioEvent *e)
{
	AudioEvent::AudioEventType type = static_cast<AudioEvent::AudioEventType>(e->type());

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::audioEventPauseState()\n");
#endif

	switch(type)
	{
		case AudioEvent::e_startPlaybackEvent:
			stopCodec(false);
			startCodec(e->uri(),e->time(),e->timeLength());
			break;
			
		case AudioEvent::e_stopPlaybackEvent:
			stopCodec();
			break;
			
		case AudioEvent::e_playPlaybackEvent:
			unpausePlayback();
			break;
			
		case AudioEvent::e_pausePlaybackEvent:
			break;
			
		case AudioEvent::e_seekPlaybackEvent:
			seekPlayback(e->time());
			break;
			
		case AudioEvent::e_setVolumeEvent:
			doSetVolume(e->volume(), e->isCallback());
			break;
						
		case AudioEvent::e_setDeviceID:
			setDeviceID(e->device());
			break;
			
		case AudioEvent::e_updateChannelMap:
			doUpdateChannelMap(e->device());
			break;
			
		case AudioEvent::e_nextPlaybackEvent:
			startNextCodec(e->uri(),e->time(),e->timeLength());
			break;
			
		case AudioEvent::e_nextCrossPlaybackEvent:
			if(m_crossFadeFlag)
			{
				startNextCodec(e->uri(),e->time(),e->timeLength(),true);
			}
			else
			{
				stopCodec(false);
				startCodec(e->uri(),e->time(),e->timeLength());
			}
			break;
			
		case AudioEvent::e_crossFadeEvent:
			doSetCrossFade(e->time());
			break;

		case AudioEvent::e_audioDeviceChangeEvent:
			audioDeviceChange();
			break;
			
		case AudioEvent::e_resetPlaybackEvent:
			resetPlayback();
			break;
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::audioEventNoCodecState(AudioEvent *e)
{
	AudioEvent::AudioEventType type = static_cast<AudioEvent::AudioEventType>(e->type());

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::audioEventNoCodecState()\n");
#endif

	switch(type)
	{
		case AudioEvent::e_startPlaybackEvent:
			stopCodec(false);
			startCodec(e->uri(),e->time(),e->timeLength());
			break;
			
		case AudioEvent::e_stopPlaybackEvent:
			stopCodec();
			break;
			
		case AudioEvent::e_playPlaybackEvent:
			break;
			
		case AudioEvent::e_pausePlaybackEvent:
			pausePlayback(true);
			break;
			
		case AudioEvent::e_seekPlaybackEvent:
			seekPlayback(e->time());
			break;
			
		case AudioEvent::e_setVolumeEvent:
			doSetVolume(e->volume(), e->isCallback());
			break;
			
		case AudioEvent::e_setDeviceID:
			setDeviceID(e->device());
			break;
			
		case AudioEvent::e_updateChannelMap:
			doUpdateChannelMap(e->device());
			break;
			
		case AudioEvent::e_nextPlaybackEvent:
			startNextCodec(e->uri(),e->time(),e->timeLength());
			break;
			
		case AudioEvent::e_nextCrossPlaybackEvent:
			if(m_crossFadeFlag)
			{
				startNextCodec(e->uri(),e->time(),e->timeLength());
			}
			else
			{
				stopCodec(false);
				startCodec(e->uri(),e->time(),e->timeLength());
			}
			break;
			
		case AudioEvent::e_crossFadeEvent:
			doSetCrossFade(e->time());
			break;

		case AudioEvent::e_audioDeviceChangeEvent:
			audioDeviceChange();
			break;
			
		case AudioEvent::e_resetPlaybackEvent:
			resetPlayback();
			break;
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::audioEventPreBufferState(AudioEvent *e)
{
	AudioEvent::AudioEventType type = static_cast<AudioEvent::AudioEventType>(e->type());

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::audioEventPreBufferState()\n");
#endif

	switch(type)
	{
		case AudioEvent::e_startPlaybackEvent:
			stopCodec();
			startCodec(e->uri(),e->time(),e->timeLength());
			break;
			
		case AudioEvent::e_stopPlaybackEvent:
			stopCodec();
			break;
			
		case AudioEvent::e_playPlaybackEvent:
			break;
			
		case AudioEvent::e_pausePlaybackEvent:
			pausePlayback(true);
			break;
			
		case AudioEvent::e_seekPlaybackEvent:
			seekPlayback(e->time());
			break;
			
		case AudioEvent::e_setVolumeEvent:
			doSetVolume(e->volume(), e->isCallback());
			break;
			
		case AudioEvent::e_setDeviceID:
			setDeviceID(e->device());
			break;

		case AudioEvent::e_updateChannelMap:
			doUpdateChannelMap(e->device());
			break;
			
		case AudioEvent::e_nextPlaybackEvent:
			startNextCodec(e->uri(),e->time(),e->timeLength());
			break;
			
		case AudioEvent::e_nextCrossPlaybackEvent:
			if(m_crossFadeFlag)
			{
				startNextCodec(e->uri(),e->time(),e->timeLength(),true);
			}
			else
			{
				stopCodec(false);
				startCodec(e->uri(),e->time(),e->timeLength());
			}
			break;
			
		case AudioEvent::e_crossFadeEvent:
			doSetCrossFade(e->time());
			break;

		case AudioEvent::e_audioDeviceChangeEvent:
			audioDeviceChange();
			break;
		
		case AudioEvent::e_resetPlaybackEvent:
			break;
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::audioEventCrossFadeState(AudioEvent *e)
{
	AudioEvent::AudioEventType type = static_cast<AudioEvent::AudioEventType>(e->type());

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::audioEventCrossFadeState()\n");
#endif

	switch(type)
	{
		case AudioEvent::e_startPlaybackEvent:
			stopCodec();
			startCodec(e->uri(),e->time(),e->timeLength());
			break;
			
		case AudioEvent::e_stopPlaybackEvent:
			stopCodec();
			break;
			
		case AudioEvent::e_playPlaybackEvent:
			break;
			
		case AudioEvent::e_pausePlaybackEvent:
			pausePlayback(true);
			break;
			
		case AudioEvent::e_seekPlaybackEvent:
			seekPlayback(e->time());
			break;
			
		case AudioEvent::e_setVolumeEvent:
			doSetVolume(e->volume(), e->isCallback());
			break;
			
		case AudioEvent::e_setDeviceID:
			setDeviceID(e->device());
			break;

		case AudioEvent::e_updateChannelMap:
			doUpdateChannelMap(e->device());
			break;
			
		case AudioEvent::e_nextPlaybackEvent:
			startNextCodec(e->uri(),e->time(),e->timeLength());
			break;
			
		case AudioEvent::e_nextCrossPlaybackEvent:
			if(m_crossFadeFlag)
			{
				startNextCodec(e->uri(),e->time(),e->timeLength());
			}
			else
			{
				stopCodec(false);
				startCodec(e->uri(),e->time(),e->timeLength());
			}
			break;
			
		case AudioEvent::e_crossFadeEvent:
			doSetCrossFade(e->time());
			break;

		case AudioEvent::e_audioDeviceChangeEvent:
			audioDeviceChange();
			break;
			
		case AudioEvent::e_resetPlaybackEvent:
			resetPlayback();
			break;
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::open(const QString& url)
{
	common::TimeStamp t;
	open(url,t);
}

//-------------------------------------------------------------------------------------------

void AOBase::open(const QString& url,const common::TimeStamp& t)
{
	common::TimeStamp len;
	open(url,t,len);
}

//-------------------------------------------------------------------------------------------

void AOBase::open(const QString& url,const common::TimeStamp& t,const common::TimeStamp& len)
{
	AudioEvent *e = new AudioEvent(AudioEvent::e_startPlaybackEvent);
	
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::open - %s, %.8f, %.8f\n", url.toUtf8().constData(), static_cast<tfloat64>(t), static_cast<tfloat64>(len));
#endif

	e->uri() = url;
	e->time() = t;
	e->timeLength() = len;
	postAudioEvent(e);
}

//-------------------------------------------------------------------------------------------

void AOBase::next(const QString& url)
{
	common::TimeStamp t;
	next(url,t);
}

//-------------------------------------------------------------------------------------------

void AOBase::next(const QString& url,const common::TimeStamp& t)
{
	common::TimeStamp len;
	next(url,t,len);
}

//-------------------------------------------------------------------------------------------

void AOBase::next(const QString& url,const common::TimeStamp& t,const common::TimeStamp& len)
{
	AudioEvent *e = new AudioEvent(AudioEvent::e_nextPlaybackEvent);

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::next - %s, %.8f, %.8f\n", url.toUtf8().constData(), static_cast<tfloat64>(t), static_cast<tfloat64>(len));
#endif

	e->uri() = url;
	e->time() = t;
	e->timeLength() = len;
	postAudioEvent(e);
}

//-------------------------------------------------------------------------------------------

void AOBase::nextCrossfade(const QString& url)
{
	common::TimeStamp t;
	nextCrossfade(url,t);
}

//-------------------------------------------------------------------------------------------

void AOBase::nextCrossfade(const QString& url,const common::TimeStamp& t)
{
	common::TimeStamp len;
	nextCrossfade(url,t,len);
}

//-------------------------------------------------------------------------------------------

void AOBase::nextCrossfade(const QString& url,const common::TimeStamp& t,const common::TimeStamp& len)
{
	AudioEvent *e = new AudioEvent(AudioEvent::e_nextCrossPlaybackEvent);

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::nextCrossfade - %s, %.8f, %.8f\n", url.toUtf8().constData(), static_cast<tfloat64>(t), static_cast<tfloat64>(len));
#endif

	e->uri() = url;
	e->time() = t;
	e->timeLength() = len;
	postAudioEvent(e);
}

//-------------------------------------------------------------------------------------------

void AOBase::play()
{
	AudioEvent *e = new AudioEvent(AudioEvent::e_playPlaybackEvent);
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::play\n");
#endif
	postAudioEvent(e);
}

//-------------------------------------------------------------------------------------------

void AOBase::pause()
{
	AudioEvent *e = new AudioEvent(AudioEvent::e_pausePlaybackEvent);
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::pause\n");
#endif
	postAudioEvent(e);
}

//-------------------------------------------------------------------------------------------

void AOBase::stop()
{
	AudioEvent *e = new AudioEvent(AudioEvent::e_stopPlaybackEvent);
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::stop\n");
#endif
	postAudioEvent(e);
}

//-------------------------------------------------------------------------------------------

void AOBase::resetPlay()
{
	AudioEvent *e = new AudioEvent(AudioEvent::e_resetPlaybackEvent);
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::reset\n");
#endif
	postAudioEvent(e);
}

//-------------------------------------------------------------------------------------------

void AOBase::seek(const common::TimeStamp& t)
{
	AudioEvent *e = new AudioEvent(AudioEvent::e_seekPlaybackEvent);
	e->time() = t;
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::seek\n");
#endif
	postAudioEvent(e);
}

//-------------------------------------------------------------------------------------------

void AOBase::setVolume(sample_t vol)
{
	AudioEvent *e = new AudioEvent(AudioEvent::e_setVolumeEvent);
	e->volume() = vol;
	e->isCallback() = false;
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::setVolume\n");
#endif
	QCoreApplication::postEvent(this,e);
}

//-------------------------------------------------------------------------------------------

sample_t AOBase::getVolume()
{
	return m_volume;
}

//-------------------------------------------------------------------------------------------

bool AOBase::isCrossfade() const
{
	return m_crossFadeFlag;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::getCrossfade() const
{
	return m_crossFadeTimeLen;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCrossfade(const common::TimeStamp& t)
{
	AudioEvent *e = new AudioEvent(AudioEvent::e_crossFadeEvent);
	e->time() = t;
	postAudioEvent(e);
}

//-------------------------------------------------------------------------------------------

bool AOBase::initResampler(int iFreq,int oFreq)
{
//#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::initResampler - input=%d, output=%d\n", iFreq, oFreq);
//#endif

	closeResampler();
	
	if(iFreq!=oFreq)
	{
		int i;
		
		m_lfeFilter = createLFEBandPassFilter(oFreq);
		
		m_resampleFlag = true;
		m_resampleItem = new AudioItem(this,c_noSamplesPerAudioItem,m_noInChannels,m_noOutChannels);
		m_resampleRatio = static_cast<tfloat64>(oFreq) / static_cast<tfloat64>(iFreq);
		m_rInFrequency = iFreq;
		m_rUsedI = 0;
		m_rUsedO = 0;
		m_rSrcLen = 4096;
		m_rDstLen = static_cast<tint>(static_cast<tfloat64>(m_rSrcLen) * m_resampleRatio) + 1000;
		m_rOutNo = 0;
		m_rOutDrift = 0.0;
		m_rCodecCompleteFlag = false;
		for(i=0;i<m_noInChannels;i++)
		{
			m_resample[i] = new engine::Resample;
			m_resample[i]->open(1,m_resampleRatio,m_resampleRatio);
			m_rIn[i] = new sample_t [m_rSrcLen];
			m_rOut[i] = new sample_t [m_rDstLen];
		}
		m_resampleList.clear();
	}
	return true;
}

//-------------------------------------------------------------------------------------------

void AOBase::closeResampler()
{
	int i;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::closeResampler\n");
#endif

	if(m_resampleItem!=0)
	{
		delete m_resampleItem;
		m_resampleItem = 0;
	}
	
	for(i=0;i<8;i++)
	{
		if(m_rIn[i]!=0)
		{
			delete [] m_rIn[i];
			m_rIn[i] = 0;
		}
		if(m_rOut[i]!=0)
		{
			delete [] m_rOut[i];
			m_rOut[i] = 0;
		}
		if(m_resample[i]!=0)
		{
			delete m_resample[i];
			m_resample[i] = 0;
		}
	}
	m_resampleFlag = false;
}

//-------------------------------------------------------------------------------------------

void AOBase::resetResampler()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::resetResampler\n");
#endif
	if(m_resampleFlag)
	{
		initResampler(m_rInFrequency,m_frequency);
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::resetResampler(int iFreq,int oFreq)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::resetResampler - %d, %d\n", iFreq, oFreq);
#endif
	if(m_resampleFlag)
	{
		initResampler(iFreq,oFreq);
	}
}

//-------------------------------------------------------------------------------------------

QSharedPointer<engine::FIRFilter> AOBase::createLFEBandPassFilter(int freq)
{
    return engine::createFIRFilter200HzLowPass(freq);
}

//-------------------------------------------------------------------------------------------

bool AOBase::decodeAndResample(engine::Codec *c,AudioItem *outputItem,bool& initF)
{
	tint i,j,k,idx;
	engine::AData& dData = *(outputItem->data());
	bool res = true;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::decodeAndResample\n");
#endif

	if(m_resampleFlag)
	{
		engine::RData *iData = dynamic_cast<engine::RData *>(m_resampleItem->data());
		engine::RData *oData = dynamic_cast<engine::RData *>(&dData);
		
		if(m_resampleList.size()>0 && m_resampleList.last().complete())
		{
			m_rCodecCompleteFlag = true;
		}

		while(oData->rLength()>0)
		{
			bool pFlag = false;
			
			while(m_resampleList.size()>0 && m_rUsedO<m_rOutNo && oData->rLength()>0)
			{
				tint dLen,rLen;
				sample_t *out;
				engine::RData::Part& p = oData->nextPart();

				dLen = decodeAndResampleCalculateOutputLength();

				rLen = p.length();
				out = oData->partData(oData->noParts() - 1);
				
                i = decodeAndResampleInterleaveOutputChannels(out,dLen,rLen);
				if(!decodeAndResampleSetCompletePartTiming(dData,p,i,dLen))
				{
					return false;
				}
				
				if(initF)
				{
					p.refStartTime() = m_refStartAudioTime;
					m_refStartAudioTime = 0;
					initF = false;
				}
				pFlag = true;
			}
			
			if(oData->rLength()>0)
			{
				while(m_rUsedI<m_rSrcLen && !m_rCodecCompleteFlag)
				{
					tint partNo,partOffset;
						
					partNo = static_cast<tint>((static_cast<tuint32>(m_resampleItem->done()) >> 22) & 0x000003ff);
					partOffset = static_cast<tint>(static_cast<tuint32>(m_resampleItem->done()) & 0x003fffff);
					
					if(partNo<iData->noParts())
					{
						sample_t *in = iData->partData(partNo);
						engine::RData::Part& p = iData->part(partNo);
						AOResampleInfo dInfo;
						
						dInfo.start() = p.start() + static_cast<tfloat64>(partOffset) / static_cast<tfloat64>(m_rInFrequency);
					
						for(i=partOffset,idx=partOffset,j=m_rUsedI;i<p.length() && j<m_rSrcLen;++i,++j)
						{
							for(k=0;k<m_noInChannels;k++,idx++)
							{
								sample_t *inC = m_rIn[k];
								inC[j] = in[idx];
							}
						}
						partOffset = i;
						m_rUsedI = j;
						
						if(partOffset>=p.length())
						{
							dInfo.end() = p.end();
							dInfo.complete() = (p.isNext()) ? true : false;
							partOffset = 0;
							partNo++;
						}
						else
						{
							dInfo.end() = p.start() + static_cast<tfloat64>(partOffset) / static_cast<tfloat64>(m_rInFrequency);
							dInfo.complete() = false;
						}
						m_resampleItem->setDone(static_cast<tint>(((static_cast<tuint32>(partNo) << 22) & 0xffc00000) | (static_cast<tuint32>(partOffset) &  0x003fffff)));
						m_resampleList.append(dInfo);
					}
					else
					{
						m_resampleItem->setDone(0);
						iData->reset();
						
						if(m_resampleList.size()>0 && m_resampleList.last().complete())
						{
							m_rCodecCompleteFlag = true;
						}
						else
						{
							if(!c->next(*iData))
							{
								if(iData->noParts()==0)
								{
									m_rCodecCompleteFlag = true;
								}
								else
								{
									iData->part(iData->noParts()-1).setNext(true);
									iData->part(iData->noParts()-1).refStartTime() = m_refStartAudioTime;
									m_refStartAudioTime = 0;
								}
							}
						}
					}
					pFlag = true;
				}
			
				if(m_rUsedO>=m_rOutNo)
				{
					tint useO = 0;
					bool last = (m_resampleList.size()>0) ? m_resampleList.last().complete() : m_rCodecCompleteFlag;
										
					for(k=0;k<m_noInChannels;k++)
					{
						useO = 0;
						m_rOutNo = m_resample[k]->process(m_resampleRatio,m_rIn[k],m_rUsedI,last,useO,m_rOut[k],m_rDstLen);
					}
					
					for(i=0,j=useO;j<m_rSrcLen;++i,++j)
					{
						for(k=0;k<m_noInChannels;k++)
						{
							sample_t *inC = m_rIn[k];
							inC[i] = inC[j];
						}
					}
					m_rUsedI = i;
					m_rUsedO = 0;
				}
				else if(!pFlag)
				{
					m_rUsedI = 0;
					m_rUsedO = m_rOutNo;
				}
			}
		}

		if(oData->noParts()>0)
		{
			oData->start() = oData->part(0).start();
			oData->end() = oData->part(oData->noParts()-1).end();
		}
	}
	else
	{
		engine::RData *oData = dynamic_cast<engine::RData *>(&dData);
		
		res = setCodecSampleFormatType(c, oData);
		if(res)
		{
			res = c->next(dData);
		}
		if(res && initF)
		{
			if(oData->noParts()>0)
			{
				oData->part(0).refStartTime() = m_refStartAudioTime;
				m_refStartAudioTime = 0;
				initF = false;
			}
		}
	}
	if(isCenterChannelGenerated())
	{
        dData.center();
	}
	if(isLFEChannelGenerated())
	{
        m_lfeFilter->process(reinterpret_cast<engine::RData *>(&dData), engine::e_lfeChannelIndex, false);
	}
	dData.mixChannels();
	return res;
}

//-------------------------------------------------------------------------------------------

tint AOBase::decodeAndResampleInterleaveOutputChannels(sample_t *out,tint dLen,tint rLen)
{
	sample_t *in[8];

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::decodeAndResampleInterleaveOutputChannels\n");
#endif

	for(tint i=0;i<getNoInChannels();i++)
	{
		in[i] = getROut(i);
	}
	return decodeAndResampleInterleaveOutputChannels(out,in,dLen,rLen);
}

//-------------------------------------------------------------------------------------------

tint AOBase::decodeAndResampleInterleaveOutputChannels(sample_t *out,sample_t **in,tint dLen,tint rLen)
{
	tint i = 0;
	tint j = getRUsedO();
	tint idx = 0;
	tint len = (dLen < rLen) ? dLen : rLen;
	tint bLen = getROutNo();
	tint noChs = getNoInChannels();


#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::decodeAndResampleInterleaveOutputChannels\n");
#endif
	while(i<len && j<bLen)
	{
		for(tint k=0;k<noChs;k++,idx++)
		{
			out[idx] = in[k][j];
		}
		i++;
		j++;
	}
	setRUsedO(j);
	return i;
}

//-------------------------------------------------------------------------------------------

bool AOBase::decodeAndResampleSetCompletePartTiming(engine::AData& dData,engine::RData::Part& p,tint idx,tint dLen)
{
	bool res = true;
	AOResampleInfo& dInfo = resampleList().first();

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::decodeAndResampleSetCompletePartTiming\n");
#endif

	p.length() = idx;
	p.done() = true;
	p.start() = dInfo.start();
	if(idx==dLen)
	{
		bool complete = dInfo.complete();
		
		p.end() = dInfo.end();
		resampleList().removeFirst();
		if(complete)
		{
			dData.end() = p.end();
			res = false;
		}
		else
		{
			if(resampleList().isEmpty() && getRCodecCompleteFlag())
			{
				res = false;
			}
		}
	}
	else
	{
		p.end() = dInfo.start() + (static_cast<tfloat64>(idx) / static_cast<tfloat64>(getFrequency()));
		dInfo.start() = p.end();
	}
	return res;
}

//-------------------------------------------------------------------------------------------

tint AOBase::decodeAndResampleCalculateOutputLength()
{
	tint dLen;
	tfloat64 dLenActual,diff,rDrift;
	AOResampleInfo& dInfo = resampleList().first();

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::decodeAndResampleCalculateOutputLength\n");
#endif

	dLenActual = static_cast<tfloat64>(dInfo.end() - dInfo.start()) * static_cast<tfloat64>(getFrequency());
	dLen = static_cast<int>(dLenActual);
	diff = dLenActual - static_cast<tfloat64>(dLen);
	if(diff >= 0.5)
	{
		dLen++;
		rDrift = getROutDrift() - (static_cast<tfloat64>(dLen) - dLenActual);
	}
	else
	{
		rDrift = getROutDrift() + (dLenActual - static_cast<tfloat64>(dLen));
	}
	
	if(rDrift >= 1.0)
	{
		rDrift -= 1.0;
		dLen++;
	}
	else if(rDrift <= -1.0)
	{
		rDrift += 1.0;
		dLen--;
	}
		
	setROutDrift(rDrift);
	return dLen;
}

//-------------------------------------------------------------------------------------------

bool AOBase::isChannelGenerated(tint inChannelIdx) const
{
	bool res = false;
	for(tint i = 0; i < c_kMaxOutputChannels && !res; i++)
	{
		if(m_outputChannelArray[i] == inChannelIdx)
		{
			res = true;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOBase::isCenterChannelGenerated() const
{
    return isChannelGenerated(engine::e_centerChannelIndex);
}

//-------------------------------------------------------------------------------------------

bool AOBase::isLFEChannelGenerated() const
{
    return (isChannelGenerated(engine::e_lfeChannelIndex) && !m_lfeFilter.isNull()) ? true : false;
}

//-------------------------------------------------------------------------------------------

void AOBase::resetLFEChannel()
{
	if(!m_lfeFilter.isNull())
	{
		m_lfeFilter->reset();
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::buildChannelMapArray()
{
	AOChannelMap *aoChannelMap = 0;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::buildChannelMapArray\n");
#endif

	m_deviceInfoMutex.lock();
	if(m_deviceIdx >= 0 && m_deviceIdx < m_deviceInfo->noDevices())
	{
		m_deviceInfo->deviceDirect(m_deviceIdx)->loadChannelMap(true);
		aoChannelMap = m_deviceInfo->deviceDirect(m_deviceIdx)->channelMap();
	}

	if(m_codec!=0 && aoChannelMap != 0)
	{
		int i,j,maxChannels;
		
		for(i=0;i<c_kMaxOutputChannels;i++)
		{
			m_outputChannelArray[i] = -1;
		}
		maxChannels = aoChannelMap->noDeviceChannels();

		if(m_codec->noChannels()==2)
		{
			QSharedPointer<AudioSettings> pSettings = AudioSettings::instance(getDeviceName(m_deviceIdx));
			bool fFlag = false,sFlag = false,rFlag = false;

			switch(aoChannelMap->stereoType())
			{
				case AOChannelMap::e_Front:
					fFlag = true;
					break;
				case AOChannelMap::e_Surround:
					sFlag = true;
					break;
				case AOChannelMap::e_Rear:
					rFlag = true;
					break;
				case AOChannelMap::e_FrontSurround:
					fFlag = true;
					sFlag = true;
					break;
				case AOChannelMap::e_FrontRear:
					fFlag = true;
					rFlag = true;
					break;
				case AOChannelMap::e_SurroundRear:
					sFlag = true;
					rFlag = true;
					break;
				case AOChannelMap::e_FrontSurroundRear:
					fFlag = true;
					sFlag = true;
					rFlag = true;
					break;
			}

			if(fFlag)
			{
				int idxL = aoChannelMap->channel(e_FrontLeft);
				int idxR = aoChannelMap->channel(e_FrontRight);
				if(idxL >= 0)
				{
					m_outputChannelArray[idxL] = 0;
				}
				if(idxR >= 0)
				{
					m_outputChannelArray[idxR] = 1;
				}
			}
			if(sFlag)
			{
				int idxL = aoChannelMap->channel(e_SurroundLeft);
				int idxR = aoChannelMap->channel(e_SurroundRight);
				if(idxL >= 0)
				{
					m_outputChannelArray[idxL] = 0;
				}
				if(idxR >= 0)
				{
					m_outputChannelArray[idxR] = 1;
				}
			}
			if(rFlag)
			{
				int idxL = aoChannelMap->channel(e_RearLeft);
				int idxR = aoChannelMap->channel(e_RearRight);
				if(idxL >= 0)
				{
					m_outputChannelArray[idxL] = 0;
				}
				if(idxR >= 0)
				{
					m_outputChannelArray[idxR] = 1;
				}
			}
			if(aoChannelMap->isStereoCenter() && aoChannelMap->channel(e_Center) >= 0 && aoChannelMap->channel(e_Center) < maxChannels)
			{
                m_outputChannelArray[aoChannelMap->channel(e_Center)] = engine::e_centerChannelIndex;
			}
			if(aoChannelMap->isStereoLFE() && aoChannelMap->channel(e_LFE) >= 0 && aoChannelMap->channel(e_LFE) < maxChannels)
			{
				m_lfeFilter = createLFEBandPassFilter(m_codec->frequency());
				if(!m_lfeFilter.isNull())
				{
					m_outputChannelArray[aoChannelMap->channel(e_LFE)] = engine::e_lfeChannelIndex;
				}
			}
		}
		else
		{
			for(i=0,j=0;i<8;i++)
			{
				int idx = aoChannelMap->channel(static_cast<ChannelType>(i));
				if(idx>=0)
				{
					m_outputChannelArray[idx] = j++;
				}
			}
		}

		for(i=maxChannels;i<c_kMaxOutputChannels;i++)
		{
			m_outputChannelArray[i] = -1;
		}
	}
	m_deviceInfoMutex.unlock();
}

//-------------------------------------------------------------------------------------------

int AOBase::noDevices()
{
	int nDev;
	
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::noDevices\n");
#endif
	m_deviceInfoMutex.lock();
	nDev = m_deviceInfo->noDevices();
	m_deviceInfoMutex.unlock();
	return nDev;
}

//-------------------------------------------------------------------------------------------

QString AOBase::deviceName(int devIdx)
{
	QString dName;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::deviceName\n");
#endif

	m_deviceInfoMutex.lock();
	if(devIdx>=0 && devIdx<m_deviceInfo->noDevices())
	{
		dName = m_deviceInfo->device(devIdx).name();
	}
	m_deviceInfoMutex.unlock();
	return dName;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<AOQueryDevice::Device> AOBase::device(int devIdx)
{
	QSharedPointer<AOQueryDevice::Device> pDevice;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::device\n");
#endif

	m_deviceInfoMutex.lock();
	if(devIdx>=0 && devIdx<m_deviceInfo->noDevices())
	{
		if(!m_deviceInfo->device(devIdx).isInitialized())
		{
			if(m_deviceInfo->queryDevice(devIdx))
			{
				pDevice = copyDeviceInformation(m_deviceInfo->device(devIdx));
			}
		}
		else
		{
			pDevice = copyDeviceInformation(m_deviceInfo->device(devIdx));
		}
	}
	m_deviceInfoMutex.unlock();
	return pDevice;
}

//-------------------------------------------------------------------------------------------

int AOBase::currentOutputDeviceIndex()
{
	return m_deviceIdx;
}

//-------------------------------------------------------------------------------------------

void AOBase::setOutputDevice(int devIdx)
{
	AudioEvent *e = new AudioEvent(AudioEvent::e_setDeviceID);
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::setOutputDevice\n");
#endif
	e->device() = devIdx;
	postAudioEvent(e);
}

//-------------------------------------------------------------------------------------------

void AOBase::updateChannelMap(int devIdx)
{
	AudioEvent *e = new AudioEvent(AudioEvent::e_updateChannelMap);
	e->device() = devIdx;
	postAudioEvent(e);
}

//-------------------------------------------------------------------------------------------

bool AOBase::isChannelMapShared(tint deviceIdx) const
{
	return false;
}

//-------------------------------------------------------------------------------------------

bool AOBase::openMergeCodec(const QString& fileName)
{
	bool res = false;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::openMergeCodec\n");
#endif

	closeMergeCodec();
	
	if(!fileName.isEmpty())
	{
		m_mergeCodec = engine::Codec::get(fileName);
		if(m_mergeCodec!=0)
		{
			m_mergeCodec->setNoOutputChannels(getNoChannelsMapped());
			if(m_mergeCodec->init())
			{
				m_mergeCodeTime = 0.0;
				res = true;
			}
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void AOBase::closeMergeCodec()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::closeMergeCodec\n");
#endif

	if(m_mergeCodec!=0)
	{
		m_mergeCodec->close();
		delete m_mergeCodec;
		m_mergeCodec = 0;
	}
}

//-------------------------------------------------------------------------------------------

bool AOBase::mergeAudioWithCodec(engine::Codec *mCodec,AudioItem *oItem)
{
	engine::RData *oData = dynamic_cast<engine::RData *>(oItem->data());
	engine::RData *mData = dynamic_cast<engine::RData *>(m_mergeAudioItem->data());
	bool res = true;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::mergeAudioWithCodec\n");
#endif

	if(m_codec!=0)
	{
		if(m_mergeCodec->noChannels()!=m_codec->noChannels() || m_mergeCodec->frequency()!=m_codec->frequency())
		{
			return false;
		}
	}
	
	if(oData!=0)
	{
		tint oPartNo,mPartNo;
		bool loop = true;
	
		while(loop)
		{
			oPartNo = static_cast<tint>((static_cast<tuint32>(oItem->done()) >> 22) & 0x000003ff);
			if(oPartNo < oData->noParts())
			{
				mPartNo = static_cast<tint>((static_cast<tuint32>(m_mergeAudioItem->done()) >> 22) & 0x000003ff);
				if(mPartNo < mData->noParts())
				{
					mergeAudioStreams(oItem,m_mergeAudioItem);
				}
				else
				{
					mData->reset();
					if(mCodec->next(*mData))
					{
						m_mergeAudioItem->setDone(0);
					}
					else
					{
						loop = false;
						res = false;
					}
				}
			}
			else
			{
				loop = false;
			}
		}
	}
	oItem->setDone(0);
	
	return res;
}

//-------------------------------------------------------------------------------------------

void AOBase::mergeAudioStreams(AudioItem *oItem,AudioItem *mItem)
{
	common::TimeStamp mergeFadeTimeLength(1.5);
	engine::RData *oData,*mData;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::mergeAudioStreams\n");
#endif

	mData = dynamic_cast<engine::RData *>(mItem->data());
	oData = dynamic_cast<engine::RData *>(oItem->data());
	
	if(oData!=0 && mData!=0 && oData->noInChannels()==mData->noInChannels())
	{
		tint i,j,k,l,mPartNo,mOffset,oPartNo,oOffset,mAmount,oAmount,tAmount,noChannels;
		sample_t *mMem,*oMem;
		
		noChannels = oData->noInChannels();
		
		mPartNo = static_cast<tint>((static_cast<tuint32>(mItem->done()) >> 22) & 0x000003ff);
		mOffset = static_cast<tint>(static_cast<tuint32>(mItem->done()) & 0x003fffff);
		
		oPartNo = static_cast<tint>((static_cast<tuint32>(oItem->done()) >> 22) & 0x000003ff);
		oOffset = static_cast<tint>(static_cast<tuint32>(oItem->done()) & 0x003fffff);
		
		i = mPartNo;
		j = oPartNo;

		while(i<mData->noParts() && j<oData->noParts())
		{
			tint fInAmount=0,fOutAmount=0,rAmount=0;
			tfloat64 fInCurrent=0.0,fOutCurrent=0.0;

			engine::RData::Part& mPart = mData->part(i);
			engine::RData::Part& oPart = oData->part(i);
			
			mMem = mData->partData(i);
			mMem = &mMem[mOffset * noChannels];
			oMem = oData->partData(i);
			oMem = &oMem[oOffset * noChannels];
			
			mAmount = mPart.length() - mOffset;
			oAmount = oPart.length() - oOffset;
			tAmount = (mAmount < oAmount) ? mAmount : oAmount;
			
			if(m_mergeCodeTime < mergeFadeTimeLength)
			{
				common::TimeStamp fTS = mergeFadeTimeLength - m_mergeCodeTime;
				tfloat64 fT = static_cast<tfloat64>(fTS);
				
				fInAmount = static_cast<tint>(fT * static_cast<tfloat64>(m_mergeCodec->frequency()));
				if(fInAmount > tAmount)
				{
					fInAmount = tAmount;
				}
				fInCurrent = 2048.0 * (static_cast<tfloat64>(m_mergeCodeTime) / static_cast<tfloat64>(mergeFadeTimeLength));

				rAmount = tAmount - fInAmount;
			}
			else
			{
				common::TimeStamp sFadeTS = m_mergeCodec->length() - mergeFadeTimeLength;
				common::TimeStamp endTS = m_mergeCodeTime + (static_cast<tfloat64>(tAmount) / static_cast<tfloat64>(m_mergeCodec->frequency()));
				
				if(sFadeTS < endTS)
				{
					if(m_mergeCodeTime < sFadeTS)
					{
						common::TimeStamp offTS = sFadeTS - m_mergeCodeTime;
						rAmount = static_cast<tint>(static_cast<tfloat64>(offTS) * static_cast<tfloat64>(m_mergeCodec->frequency()));
					}
					else
					{
						common::TimeStamp offTS = m_mergeCodeTime - sFadeTS;
						fOutCurrent = 2048.0 * (static_cast<tfloat64>(offTS) / static_cast<tfloat64>(mergeFadeTimeLength));
					}
					fOutAmount = tAmount - rAmount;
				}
				else
				{
					rAmount = tAmount;
				}
			}

			tfloat64 fInc = 2048.0 / (static_cast<tfloat64>(mergeFadeTimeLength) * static_cast<tfloat64>(m_mergeCodec->frequency()));
			
			for(k=0;k<fInAmount;k++,fInCurrent+=fInc)
			{
				tint inc = static_cast<tint>(fInCurrent);
				if(inc<0)
				{
					inc = 0;
				}
				else if(inc>=2048)
				{
					inc = 2047;
				}
				
				for(l=0;l<noChannels;l++,oMem++,mMem++)
				{
#if defined(SINGLE_FLOAT_SAMPLE)
					*oMem = (0.3f * (*oMem)) + (0.7f * ((m_crossFadeAWin[inc] * (*oMem)) + (m_crossFadeBWin[inc] * (*mMem))));
#else
					*oMem = (0.3 * (*oMem)) + (0.7 * ((m_crossFadeAWin[inc] * (*oMem)) + (m_crossFadeBWin[inc] * (*mMem))));
#endif
				}
			}
			
			for(k=0;k<rAmount;k++)
			{
				for(l=0;l<noChannels;l++,oMem++,mMem++)
				{
#if defined(SINGLE_FLOAT_SAMPLE)
					*oMem = (0.3f * (*oMem)) + (0.7f * (*mMem));
#else
					*oMem = (0.3 * (*oMem)) + (0.7 * (*mMem));
#endif
				}
			}
			
			for(k=0;k<fOutAmount;k++,fOutCurrent+=fInc)
			{
				tint inc = static_cast<tint>(fOutCurrent);
				if(inc<0)
				{
					inc = 0;
				}
				else if(inc>=2048)
				{
					inc = 2047;
				}
				
				for(l=0;l<noChannels;l++,oMem++,mMem++)
				{
#if defined(SINGLE_FLOAT_SAMPLE)
					*oMem = (0.3f * (*oMem)) + (0.7f * ((m_crossFadeBWin[inc] * (*oMem)) + (m_crossFadeAWin[inc] * (*mMem))));
#else
					*oMem = (0.3 * (*oMem)) + (0.7 * ((m_crossFadeBWin[inc] * (*oMem)) + (m_crossFadeAWin[inc] * (*mMem))));
#endif
				}				
			}
			
			mOffset += tAmount;
			oOffset += tAmount;
			
			m_mergeCodeTime += static_cast<tfloat64>(tAmount) / static_cast<tfloat64>(m_mergeCodec->frequency());

			if(mPart.length() >= mOffset)
			{
				mOffset = 0;
				i++;
			}
			if(oPart.length() >= oOffset)
			{
				oOffset = 0;
				j++;
			}
		}
		
		mItem->setDone(static_cast<tint>(((static_cast<tuint32>(i) << 22) & 0xffc00000) | (static_cast<tuint32>(mOffset) &  0x003fffff)));
		oItem->setDone(static_cast<tint>(((static_cast<tuint32>(j) << 22) & 0xffc00000) | (static_cast<tuint32>(oOffset) &  0x003fffff)));
	}
}

//-------------------------------------------------------------------------------------------
// IO Soundcard callback methods
//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::getReferenceClockTime() const
{
	return common::TimeStamp::reference();
}

//-------------------------------------------------------------------------------------------

void AOBase::incrementMutexCount()
{
	m_mutexCount++;
}

//-------------------------------------------------------------------------------------------

tint AOBase::getMutexCount() const
{
	return m_mutexCount;
}

//-------------------------------------------------------------------------------------------

void AOBase::updateCurrentPlayTimeFromStreamTime(const IOTimeStamp& systemTime)
{
	if(systemTime.isValid())
	{
		common::TimeStamp playTime;
		
		if(!getAudioStartFlag())
		{
			common::TimeStamp startTime = systemTime.time() - getPauseTime();
			setAudioStartClock(startTime);
			setAudioStartFlag(true);
			
			if(!getTrackTimeStateFlag())
			{
				setTrackTimeState(1);
			}
			setTrackTimeStateFlag(false);
		}
		playTime = systemTime.time() - getAudioStartClock();
		setCurrentPlayTime(playTime);

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::updateCurrentPlayTimeFromStreamTime - %.8f, %.8f\n", static_cast<tfloat64>(systemTime.time()), static_cast<tfloat64>(playTime));
#endif
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::setItemStateToCallbackAsApplicable(AudioItem *item)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::setItemStateToCallbackAsApplicable\n");
#endif

	if(item->state()==AudioItem::e_stateFull || item->state()==AudioItem::e_stateFullEnd)
	{
		if(item->state()==AudioItem::e_stateFull)
		{
			item->setState(AudioItem::e_stateCallback);
		}
		else
		{
			item->setState(AudioItem::e_stateCallbackEnd);
		}
		setCallbackAudioTime(item);
	}
}

//-------------------------------------------------------------------------------------------

tint AOBase::remainingSamplesInBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
	return pBuffer->bufferLength() - fromIndex;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::lengthOfTime(tint noSamples) const
{
	common::TimeStamp iT(static_cast<tfloat64>(noSamples)/static_cast<tfloat64>(getFrequency()));
	return iT;
}

//-------------------------------------------------------------------------------------------

tint AOBase::partBufferIndexForChannel(tint channelIndex) const
{
	tint bufferIndex = -1;
	
	if(channelIndex>=0 && channelIndex<c_kMaxOutputChannels)
	{
		bufferIndex = m_outputChannelArray[channelIndex];
	}
	return bufferIndex;
}

//-------------------------------------------------------------------------------------------

void AOBase::playbackOfNextTrackIsStarting(const engine::RData::Part& part,const IOTimeStamp& systemTime,tint sIndex)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::playbackOfNextTrackIsStarting\n");
#endif

	if(part.isNext())
	{
		common::TimeStamp aSClock;
        aSClock = systemTime.time() + lengthOfTime(sIndex) - part.startConst();
		setAudioStartClock(aSClock);
        setCurrentOutTime(part.startConst());
        setCurrentPlayTime(part.startConst());
        setCurrentCallbackTime(part.startConst());
		if(!getTrackTimeStateFlag())
		{
			setTrackTimeState(1);
		}
		setTrackTimeStateFlag(false);
		setStartNextTrackFlag(true);
	}
}

//-------------------------------------------------------------------------------------------

tint AOBase::numberOfSamplesInTime(common::TimeStamp& t) const
{
	tfloat64 freq = static_cast<tfloat64>(getFrequency());
	tfloat64 actualNoSamples = static_cast<tfloat64>(t) * freq;
	tint noWholeSamples = static_cast<tint>(actualNoSamples);
	t = static_cast<tfloat64>(noWholeSamples) / freq;
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::numberOfSamplesInTime - %.8f, %d\n", static_cast<tfloat64>(t), noWholeSamples);
#endif
	return noWholeSamples;
}

//-------------------------------------------------------------------------------------------

tint AOBase::numberOfSamplesInFixedTime(const common::TimeStamp& t) const
{
	tfloat64 freq = static_cast<tfloat64>(getFrequency());
	tfloat64 actualNoSamples = static_cast<tfloat64>(t) * freq;
	tint noWholeSamples = static_cast<tint>(actualNoSamples);
	tfloat64 diff = actualNoSamples - static_cast<tfloat64>(noWholeSamples);
	if(diff > 0.5)
	{
		noWholeSamples++;
	}
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::numberOfSamplesInFixedTime - %.8f, %d\n", static_cast<tfloat64>(t), noWholeSamples);
#endif
	return noWholeSamples;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::timeForNumberOfSamples(tint numberOfSamples) const
{
	common::TimeStamp t(static_cast<tfloat64>(numberOfSamples) / static_cast<tfloat64>(getFrequency()));
	return t;
}

//-------------------------------------------------------------------------------------------

void AOBase::syncAudioTimeToPartReferenceLatencyDelay(engine::RData::Part& part,const IOTimeStamp& systemTime,const common::TimeStamp& referenceTime)
{
	common::TimeStamp dT,currentOutT,audioStartT;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::syncAudioTimeToPartReferenceLatencyDelay\n");
#endif

	dT = referenceTime - part.refStartTime();
	currentOutT = dT + part.start();
	setCurrentOutTime(currentOutT);
	if(systemTime.isValid())
	{
		audioStartT = systemTime.time() - currentOutT;
		setAudioStartClock(audioStartT);
	}
	setCurrentPlayTime(currentOutT);
	part.refStartTime() = 0;
}

//-------------------------------------------------------------------------------------------

void AOBase::writeSilenceForSynchronizedLatencyDelay(AbstractAudioHardwareBuffer *pBuffer,engine::RData::Part& part,const common::TimeStamp& referenceTime,tint& outputSampleIndex)
{
	tint amount;
	common::TimeStamp dT,currentOutT;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::writeSilenceForSynchronizedLatencyDelay\n");
#endif

	dT = part.refStartTime() - referenceTime;
	amount = numberOfSamplesInTime(dT);
	if(amount > (pBuffer->bufferLength() - outputSampleIndex))
	{
        amount = pBuffer->bufferLength() - outputSampleIndex;
		dT = timeForNumberOfSamples(amount);
	}
	
	writeToAudioSilenceForGivenRange(pBuffer,outputSampleIndex,amount);
	
	outputSampleIndex += amount;
	currentOutT = getCurrentOutTime() + dT;
	setCurrentOutTime(currentOutT);
}

//-------------------------------------------------------------------------------------------

void AOBase::syncAudioToPartReferenceLatencyDelay(AbstractAudioHardwareBuffer *pBuffer,engine::RData::Part& part,const IOTimeStamp& systemTime,tint& outputSampleIndex)
{
	common::TimeStamp referenceTime = getReferenceClockTime();

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::syncAudioToPartReferenceLatencyDelay\n");
#endif

	if(referenceTime >= part.refStartTime())
	{
		syncAudioTimeToPartReferenceLatencyDelay(part,systemTime,referenceTime);
	}
	else
	{
		writeSilenceForSynchronizedLatencyDelay(pBuffer,part,referenceTime,outputSampleIndex);
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::writeToAudioSilenceForGivenRange(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex,tint numberOfSamples)
{
	if(numberOfSamples > 0)
	{
		tint j,k;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::writeToAudioSilenceForGivenRange - %d, %d\n", fromIndex, numberOfSamples);
#endif
	
		j = 0;
		k = 0;
		while(k < pBuffer->numberOfBuffers())
		{
			tbyte *out = pBuffer->buffer(k);
			tint noChs = pBuffer->numberOfChannelsInBuffer(k);
			tint sampleSize = pBuffer->sampleSize(k);
			memset(&out[fromIndex * noChs * sampleSize],0,numberOfSamples * noChs * sampleSize);
			j += noChs;
			k++;
		}
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::writeToAudioSilenceToEndOfBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::writeToAudioSilenceToEndOfBuffer - %d\n", fromIndex);
#endif
	writeToAudioSilenceForGivenRange(pBuffer,fromIndex,remainingSamplesInBuffer(pBuffer,fromIndex));
}

//-------------------------------------------------------------------------------------------

void AOBase::writeToAudioSilenceForRemainder(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
	common::TimeStamp dT,newOutTime;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::writeToAudioSilenceForRemainder - %d\n", fromIndex);
#endif
	writeToAudioSilenceToEndOfBuffer(pBuffer,fromIndex);
	dT = static_cast<tfloat64>(remainingSamplesInBuffer(pBuffer,fromIndex)) / static_cast<tfloat64>(getFrequency());
	newOutTime = getCurrentOutTime() + dT;
	setCurrentOutTime(newOutTime);
}

//-------------------------------------------------------------------------------------------

void AOBase::writeToAudioOutputBufferSilence(AbstractAudioHardwareBuffer *pBuffer,tint bufferIndex,tint outChannelIndex,tint outputSampleIndex,tint amount)
{
	tbyte *out = pBuffer->buffer(bufferIndex);
	tint oIdx = ((outputSampleIndex * pBuffer->numberOfChannelsInBuffer(bufferIndex)) + outChannelIndex) * pBuffer->sampleSize(bufferIndex);
	
	tint sampleSize = pBuffer->sampleSize(bufferIndex);
	tint inc = pBuffer->numberOfChannelsInBuffer(bufferIndex) * sampleSize;
	tint tAmount = oIdx + (amount * inc);

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::writeToAudioOutputBufferSilence\n");
#endif

	while(oIdx < tAmount)
	{
		for(tint i=0;i<sampleSize;i++)
		{
			out[oIdx + i] = '\0';
		}
		oIdx += inc;
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::writeToAudioOutputBuffer(AbstractAudioHardwareBuffer *pBuffer,
                                      engine::RData *data,
                                      tint partNumber,
                                      tint inputSampleIndex,
                                      tint outputSampleIndex,
                                      tint amount)
{
	tint channelIdx,bufferIdx;

	channelIdx = 0;
	bufferIdx = 0;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::writeToAudioOutputBuffer\n");
#endif

	while(bufferIdx<pBuffer->numberOfBuffers())
	{
		for(tint packetInFrame=0;packetInFrame<pBuffer->numberOfChannelsInBuffer(bufferIdx);packetInFrame++)
		{
			tint inChannelIndex = partBufferIndexForChannel(channelIdx);
			
			if(inChannelIndex >= 0 || inChannelIndex == engine::e_centerChannelIndex || inChannelIndex == engine::e_lfeChannelIndex)
			{
				writeToAudioOutputBufferFromPartData(pBuffer,data,partNumber,inChannelIndex,
					bufferIdx,packetInFrame,inputSampleIndex,outputSampleIndex,amount);
			}
			else
			{
				writeToAudioOutputBufferSilence(pBuffer,bufferIdx,packetInFrame,outputSampleIndex,amount);
			}
			channelIdx++;
		}
		bufferIdx++;
	}
}

//-------------------------------------------------------------------------------------------

tint AOBase::partNumberFromAudioItem(AudioItem *item) const
{
	return static_cast<tint>((static_cast<tuint32>(item->done()) >> 22) & 0x000003ff);
}

//-------------------------------------------------------------------------------------------

engine::RData::Part& AOBase::partFromAudioItem(AudioItem *item) const
{
	engine::RData *data = reinterpret_cast<engine::RData *>(item->data());
    return data->part(partNumberFromAudioItem(item));
}

//-------------------------------------------------------------------------------------------

tint AOBase::offsetFromAudioItem(AudioItem *item) const
{
	tint offset;
	tuint32 t;
	
	t = static_cast<tuint32>(item->done()) & 0x001fffff;
	if(item->done() & 0x00200000)
	{
        t |= 0xffe00000;
	}
	offset = static_cast<tint>(t);
	
	if(!offset)
	{
		const engine::RData *data = reinterpret_cast<const engine::RData *>(item->dataConst());
        common::TimeStamp diffT = getCurrentOutTime() - data->partConst(partNumberFromAudioItem(item)).startConst();
		offset = numberOfSamplesInTime(diffT);
	}
	return offset;
}

//-------------------------------------------------------------------------------------------

void AOBase::setOffsetAndPartToAudioItem(AudioItem *item,tint offset,tint partNumber) const
{
	item->setDone(static_cast<tint>(((static_cast<tuint32>(partNumber) << 22) & 0xffc00000) | (static_cast<tuint32>(offset) &  0x003fffff)));
}

//-------------------------------------------------------------------------------------------

tint AOBase::writeToAudioProcessPart(AbstractAudioHardwareBuffer *pBuffer,AudioItem *item,int outputSampleIndex)
{
	tint offset;
	tint partNumber = partNumberFromAudioItem(item);
	engine::RData::Part& part = partFromAudioItem(item);

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::writeToAudioProcessPart\n");
#endif

	if(getCurrentOutTime() < part.end())
	{
		offset = offsetFromAudioItem(item);
		
		if(offset>=0 && offset<part.length())
		{
			tint amount;
			
			amount = part.length() - offset;
				
			if(amount > (pBuffer->bufferLength() - outputSampleIndex))
			{
				amount = pBuffer->bufferLength() - outputSampleIndex;
			}
			
			if(amount > 0)
			{
				common::TimeStamp currentOutT;
				engine::RData *data = reinterpret_cast<engine::RData *>(item->data());
				
				writeToAudioOutputBuffer(pBuffer,data,partNumber,offset,outputSampleIndex,amount);
				
				currentOutT = getCurrentOutTime() + timeForNumberOfSamples(amount);
				setCurrentOutTime(currentOutT);
				outputSampleIndex += amount;
			}
			
			offset += amount;
			
			if(offset >= part.length())
			{
				setCurrentOutTime(part.end());
				offset = 0;
				partNumber++;
			}
		}
		else
		{
			if(offset < 0)
			{
				setCurrentOutTime(part.start());
				offset = 0;
			}
			else
			{
				setCurrentOutTime(part.end());
				offset = 0;
				partNumber++;
			}
		}
	}
	else
	{
		offset = 0;
		partNumber++;
	}
	setOffsetAndPartToAudioItem(item,offset,partNumber);
	
	return outputSampleIndex;
}

//-------------------------------------------------------------------------------------------

AudioItem *AOBase::audioItemCallbackIsDone(AudioItem *item,tint outputSampleIndex,bool& loop,bool& loopFlag)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::audioItemCallbackIsDone\n");
#endif

	if(item->state()==AudioItem::e_stateCallbackEnd)
	{
		common::TimeStamp sT;
		
		sT = getReferenceClockTime() + getOutputLatencyTime() + timeForNumberOfSamples(outputSampleIndex);
		setStopTimeClock(sT);
		setStopTimeFlag(true);
		loop = false;
	}
	item->setState(AudioItem::e_stateDone);
	item = item->next();
	loopFlag = true;
	return item;
}

//-------------------------------------------------------------------------------------------

tint AOBase::writeToAudioSilenceUntilStartOfNextPart(AbstractAudioHardwareBuffer *pBuffer,const engine::RData::Part& part,tint outputSampleIndex)
{
	tint amount;
    common::TimeStamp dT;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::writeToAudioSilenceUntilStartOfNextPart\n");
#endif

	dT = part.startConst() - getCurrentOutTime();
	amount = numberOfSamplesInFixedTime(dT);
	if(amount > (pBuffer->bufferLength() - outputSampleIndex))
	{
		amount = pBuffer->bufferLength() - outputSampleIndex;
		dT = getCurrentOutTime() + timeForNumberOfSamples(amount);
	}
	else
	{
		dT = part.startConst();
	}
	
	writeToAudioSilenceForGivenRange(pBuffer,outputSampleIndex,amount);	
	setCurrentOutTime(dT);
	return outputSampleIndex + amount;
}

//-------------------------------------------------------------------------------------------

void AOBase::processDataForOutput(engine::RData *)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::processDataForOutput\n");
#endif
}

//-------------------------------------------------------------------------------------------

AudioItem *AOBase::writeToAudioFromItem(AbstractAudioHardwareBuffer *pBuffer,AudioItem *item,const IOTimeStamp& systemTime,tint& outputSampleIndex,bool& loop,bool& loopFlag)
{
	tint pNo;
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::writeToAudioFromItem\n");
#endif	

	processDataForOutput(data);
	
	pNo = partNumberFromAudioItem(item);
	if(pNo < data->noParts())
	{
		engine::RData::Part& part = data->part(pNo);
		
		playbackOfNextTrackIsStarting(part,systemTime,outputSampleIndex);
				
		if(part.refStartTime()!=0)
		{
			syncAudioToPartReferenceLatencyDelay(pBuffer,part,systemTime,outputSampleIndex);
		}
		else if(getCurrentOutTime() >= part.start())
		{
			outputSampleIndex = writeToAudioProcessPart(pBuffer,item,outputSampleIndex);
		}
		else
		{
			outputSampleIndex = writeToAudioSilenceUntilStartOfNextPart(pBuffer,part,outputSampleIndex);
		}
	}
	else
	{
		item = audioItemCallbackIsDone(item,outputSampleIndex,loop,loopFlag);
	}
	return item;
}

//-------------------------------------------------------------------------------------------

void AOBase::resyncAudioOutputTimeToItem(AudioItem *item)
{
	tint pNo = partNumberFromAudioItem(item);
	engine::RData *partData = dynamic_cast<engine::RData *>(item->data());
	const engine::RData::Part& part = partData->part(pNo);
	setCurrentOutTime(part.startConst());
	m_silenceIsWritten = false;	
}

//-------------------------------------------------------------------------------------------

void AOBase::writeToAudio(AbstractAudioHardwareBuffer *pBuffer,const IOTimeStamp& systemTime)
{
	tint outputSampleIndex;
	AudioItem *item = getCallbackAudioItem(), *oItem = getCallbackAudioItem();
	bool loop = true,loopFlag = false;
	
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::writeToAudio\n");
#endif

	outputSampleIndex = 0;
	
	while(outputSampleIndex<pBuffer->bufferLength() && loop && !(loopFlag && item==oItem))
	{
		setItemStateToCallbackAsApplicable(item);
		
		if(item->state()==AudioItem::e_stateCallback || item->state()==AudioItem::e_stateCallbackEnd)
		{
            if(!m_syncAudioToTimestamp && m_silenceIsWritten)
            {
            	resyncAudioOutputTimeToItem(item);
            }
			item = writeToAudioFromItem(pBuffer,item,systemTime,outputSampleIndex,loop,loopFlag);
		}
		else if(item->state()==AudioItem::e_stateDone)
		{
			item = item->next();
			loopFlag = true;
		}
		else
		{
			break;
		}
	}
	
	if(outputSampleIndex < pBuffer->bufferLength())
	{
		writeToAudioSilenceForRemainder(pBuffer,outputSampleIndex);
        if(!m_syncAudioToTimestamp && !m_silenceIsWritten)
        {
            m_silenceIsWritten = true;
        }
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::writeToAudioIOCallback(AbstractAudioHardwareBuffer *pBuffer,const IOTimeStamp& systemTime)
{
	States state = getState();

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::writeToAudioIOCallback\n");
#endif

	incrementMutexCount();
	updateCurrentPlayTimeFromStreamTime(systemTime);
	
	if(getAudioStartFlag() && (state==e_statePlay || state==e_statePreBuffer || state==e_stateCrossFade || state==e_stateNoCodec))
	{
        writeToAudio(pBuffer,systemTime);
	}
	else
	{
        writeToAudioSilenceToEndOfBuffer(pBuffer,0);
	}
	writeToAudioPostProcess();
	
	setCurrentCallbackTime(getCurrentOutTime());
	incrementMutexCount();	
}

//-------------------------------------------------------------------------------------------

void AOBase::writeToAudioPostProcess()
{}

//-------------------------------------------------------------------------------------------
// End of IO Soundcard callback methods
//-------------------------------------------------------------------------------------------
// Functionality for getting and setting the exclusive access mode
//-------------------------------------------------------------------------------------------

bool AOBase::isExclusive()
{
	return isExclusive(m_deviceIdx);
}

//-------------------------------------------------------------------------------------------

bool AOBase::isExclusive(int devIdx)
{
	return AudioSettings::instance(getDeviceName(devIdx))->isExclusive();
}

//-------------------------------------------------------------------------------------------
// End of functionality for getting and setting the exclusive access mode
//-------------------------------------------------------------------------------------------

QSharedPointer<AOQueryDevice::Device> AOBase::getCurrentDevice()
{
	int dIndex;
    QSharedPointer<AOQueryDevice::Device> pDevice;
	
	m_deviceInfoMutex.lock();
	
	dIndex = currentOutputDeviceIndex();
	if(!(dIndex>=0 && dIndex<getDeviceInfo()->noDevices()))
	{
		dIndex = getDeviceInfo()->defaultDeviceIndex();
		if(dIndex>=0 && dIndex<getDeviceInfo()->noDevices())
		{
			setDeviceIndex(dIndex);
		}
		else
		{
			dIndex = -1;
		}
	}
	
	if(dIndex>=0)
	{
		if(!getDeviceInfo()->device(dIndex).isInitialized())
		{
			if(!getDeviceInfo()->queryDevice(dIndex))
			{
				dIndex = -1;
			}
		}
		if(dIndex>=0)
		{
			pDevice = copyDeviceInformation(getDeviceInfo()->device(dIndex));
		}
	}
	
	m_deviceInfoMutex.unlock();
	return pDevice;
}

//-------------------------------------------------------------------------------------------

FormatDescription AOBase::getSourceDescription(tint noChannels)
{
	FormatDescription desc;

	if(getCodec() != 0)
	{
		if(getCodec()->dataTypesSupported() & engine::e_SampleInt32)
		{
			FormatDescription descTmp(FormatDescription::e_DataSignedInteger, 32, noChannels, getFrequency());
			desc = descTmp;
		}
		else if(getCodec()->dataTypesSupported() & engine::e_SampleInt24)
		{
			FormatDescription descTmp(FormatDescription::e_DataSignedInteger, 24, noChannels, getFrequency());
			desc = descTmp;
		}
		else if(getCodec()->dataTypesSupported() & engine::e_SampleInt16)
		{
			FormatDescription descTmp(FormatDescription::e_DataSignedInteger, 16, noChannels, getFrequency());
			desc = descTmp;
		}
		else
		{
			FormatDescription descTmp(FormatDescription::e_DataFloatDouble, 64, noChannels, getFrequency());
			desc = descTmp;
		}
	}
	else
	{
		FormatDescription descTmp(FormatDescription::e_DataFloatDouble, 64, noChannels, getFrequency());
		desc = descTmp;
	}
	return desc;
}

//-------------------------------------------------------------------------------------------

bool AOBase::isNextCodecSeamless()
{
	bool isSeamless = false;
	
	if(getCodec() != 0 && getNextCodec() != 0)
	{
		if(getCodec()->frequency() == getNextCodec()->frequency() && getNextCodec()->noChannels() == getNoInChannels())
		{
			engine::CodecDataType xorType = getCodec()->dataTypesSupported() ^ getNextCodec()->dataTypesSupported();
			if(!xorType)
			{
				isSeamless = true;
			}
		}
	}
	return isSeamless;
}

//-------------------------------------------------------------------------------------------

void AOBase::audioDeviceChange()
{}

//-------------------------------------------------------------------------------------------

void AOBase::emitOnReadyForNext()
{
	emit onReadyForNext();
}

//-------------------------------------------------------------------------------------------

void AOBase::emitOnCrossfade()
{
	emit onCrossfade();
}

//-------------------------------------------------------------------------------------------

Qt::HANDLE AOBase::threadId()
{
	return m_threadId;
}

//-------------------------------------------------------------------------------------------

void AOBase::emitOnVolumeChanged(tfloat64 vol)
{
	emit onVolumeChanged(vol);
}

//-------------------------------------------------------------------------------------------
// Member variable setters and getters
//-------------------------------------------------------------------------------------------

AOBase::States AOBase::getState() const
{
	return m_state;
}

//-------------------------------------------------------------------------------------------

void AOBase::setState(States s)
{
	m_state = s;
}

//-------------------------------------------------------------------------------------------

AOBase::CodecState AOBase::getCodecState() const
{
	return m_codecState;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCodecState(CodecState s)
{
	m_codecState = s;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AOBase::getStartCodecSeekTime() const
{
	return m_startCodecSeekTime;
}

//-------------------------------------------------------------------------------------------

void AOBase::setStartCodecSeekTime(const common::TimeStamp& t)
{
	m_startCodecSeekTime = t;
}

//-------------------------------------------------------------------------------------------

engine::Codec *AOBase::getCodec()
{
	return m_codec;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCodec(engine::Codec *c)
{
	m_codec = c;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AOBase::getNextCodecSeekTime() const
{
	return m_nextCodecSeekTime;
}

//-------------------------------------------------------------------------------------------

void AOBase::setNextCodecSeekTime(const common::TimeStamp& t)
{
	m_nextCodecSeekTime = t;
}

//-------------------------------------------------------------------------------------------

engine::Codec *AOBase::getNextCodec()
{
	return m_nextCodec;
}

//-------------------------------------------------------------------------------------------

void AOBase::setNextCodec(engine::Codec *c)
{
	m_nextCodec = c;
}

//-------------------------------------------------------------------------------------------

tint AOBase::getFrequency() const
{
	return m_frequency;
}

//-------------------------------------------------------------------------------------------

void AOBase::setFrequency(tint f)
{
	m_frequency = f;
}

//-------------------------------------------------------------------------------------------

tint AOBase::getNoInChannels() const
{
	return m_noInChannels;
}

//-------------------------------------------------------------------------------------------

void AOBase::setNoInChannels(tint n)
{
	m_noInChannels = n;
}

//-------------------------------------------------------------------------------------------

const QString& AOBase::getNextName() const
{
	return m_nextName;
}

//-------------------------------------------------------------------------------------------

void AOBase::setNextName(const QString& n)
{
	m_nextName = n;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AOBase::getCrossFadeTime() const
{
	return m_crossFadeTime;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCrossFadeTime(const common::TimeStamp& t)
{
	m_crossFadeTime = t;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AOBase::getCodecCurrentTime() const
{
	return m_codecCurrentTime;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCodecCurrentTime(const common::TimeStamp& t)
{
	m_codecCurrentTime = t;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AOBase::getProgFadeTime() const
{
	return m_progFadeTime;
}

//-------------------------------------------------------------------------------------------

void AOBase::setProgFadeTime(const common::TimeStamp& t)
{
	m_progFadeTime = t;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AOBase::getCodecTimeLength() const
{
	return m_codecTimeLength;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCodecTimeLength(const common::TimeStamp& t)
{
	m_codecTimeLength = t;
}

//-------------------------------------------------------------------------------------------

bool AOBase::getCodecTimeLengthUpdate() const
{
	return m_codecTimeLengthUpdate;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCodecTimeLengthUpdate(bool f)
{
	m_codecTimeLengthUpdate = f;
}

//-------------------------------------------------------------------------------------------

tint AOBase::getNextOutState() const
{
	return m_nextOutState;
}

//-------------------------------------------------------------------------------------------

void AOBase::setNextOutState(tint s)
{
	m_nextOutState = s;
}

//-------------------------------------------------------------------------------------------

AudioItem *AOBase::getCodecAudioItem()
{
	return m_codecAudioItem;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCodecAudioItem(AudioItem *item)
{
	m_codecAudioItem = item;	
}

//-------------------------------------------------------------------------------------------

AudioItem *AOBase::getCallbackAudioItem()
{
	return m_callbackAudioItem;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCallbackAudioTime(AudioItem *item)
{
	m_callbackAudioItem = item;
}

//-------------------------------------------------------------------------------------------

tint AOBase::getAudioProcessType()
{
	return m_audioProcessType;
}

//-------------------------------------------------------------------------------------------

void AOBase::setAudioProcessType(tint pType)
{
	m_audioProcessType = pType;
}

//-------------------------------------------------------------------------------------------

tint AOBase::getTrackTimeState() const
{
	return m_trackTimeState;
}

//-------------------------------------------------------------------------------------------

void AOBase::setTrackTimeState(tint state)
{
	m_trackTimeState = state;
}

//-------------------------------------------------------------------------------------------

bool AOBase::getTrackTimeStateFlag() const
{
	return m_trackTimeStateFlag;
}

//-------------------------------------------------------------------------------------------

void AOBase::setTrackTimeStateFlag(bool flag)
{
	m_trackTimeStateFlag = flag;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::getRefStartAudioTime() const
{
	return m_refStartAudioTime;
}

//-------------------------------------------------------------------------------------------

void AOBase::setRefStartAudioTime(const common::TimeStamp& t)
{
	m_refStartAudioTime = t;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::getNextCodecTime()
{
	return m_nextCodecTime;
}

//-------------------------------------------------------------------------------------------

void AOBase::setNextCodecTime(const common::TimeStamp& t)
{
	m_nextCodecTime = t;
}

//-------------------------------------------------------------------------------------------

bool AOBase::getCrossFadeFlag()
{
	return m_crossFadeFlag;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCrossFadeFlag(bool flag)
{
	m_crossFadeFlag = flag;
}

//-------------------------------------------------------------------------------------------

AudioItem *AOBase::getCrossFadeItem()
{
	return m_crossFadeItem;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCrossFadeItem(AudioItem *item)
{
	m_crossFadeItem = item;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::getFrameFadeTime()
{
	return m_frameFadeTime;
}

//-------------------------------------------------------------------------------------------

void AOBase::setFrameFadeTime(const common::TimeStamp& t)
{
	m_frameFadeTime = t;
}

//-------------------------------------------------------------------------------------------

bool AOBase::getAudioStartFlag()
{
	return m_audioStartFlag;
}

//-------------------------------------------------------------------------------------------

void AOBase::setAudioStartFlag(bool f)
{
	m_audioStartFlag = f;
}

//-------------------------------------------------------------------------------------------

const bool& AOBase::getResampleFlag() const
{
	return m_resampleFlag;
}

//-------------------------------------------------------------------------------------------

void AOBase::setResampleFlag(bool flag)
{
	m_resampleFlag = flag;
}

//-------------------------------------------------------------------------------------------

const tfloat64& AOBase::getResampleRatio() const
{
	return m_resampleRatio;
}

//-------------------------------------------------------------------------------------------

void AOBase::setResampleRatio(tfloat64 v)
{
	m_resampleRatio = v;
}

//-------------------------------------------------------------------------------------------

AudioItem *AOBase::getResampleItem()
{
	return m_resampleItem;
}

//-------------------------------------------------------------------------------------------

void AOBase::setResampleItem(AudioItem *item)
{
	if(m_resampleItem!=0)
	{
		delete m_resampleItem;
	}
	m_resampleItem = item;
}

//-------------------------------------------------------------------------------------------

engine::Resample *AOBase::getResampler(tint ch)
{
	return m_resample[ch];
}

//-------------------------------------------------------------------------------------------

void AOBase::setResampler(engine::Resample *resampler,tint ch)
{
	if(m_resample[ch]!=0)
	{
		delete m_resample[ch];
	}
	m_resample[ch] = resampler;
}

//-------------------------------------------------------------------------------------------

const tint& AOBase::getRInFrequency() const
{
	return m_rInFrequency;
}

//-------------------------------------------------------------------------------------------

void AOBase::setRInFrequency(tint freq)
{
	m_rInFrequency = freq;
}

//-------------------------------------------------------------------------------------------

const tint& AOBase::getRUsedI() const
{
	return m_rUsedI;
}

//-------------------------------------------------------------------------------------------

void AOBase::setRUsedI(tint usedI)
{
	m_rUsedI = usedI;
}

//-------------------------------------------------------------------------------------------

const tint& AOBase::getRUsedO() const
{
	return m_rUsedO;
}

//-------------------------------------------------------------------------------------------

void AOBase::setRUsedO(tint usedO)
{
	m_rUsedO = usedO;
}

//-------------------------------------------------------------------------------------------

const tint& AOBase::getRSrcLen() const
{
	return m_rSrcLen;
}

//-------------------------------------------------------------------------------------------

void AOBase::setRSrcLen(tint len)
{
	m_rSrcLen = len;
}

//-------------------------------------------------------------------------------------------

const tint& AOBase::getRDstLen() const
{
	return m_rDstLen;
}

//-------------------------------------------------------------------------------------------

void AOBase::setRDstLen(tint len)
{
	m_rDstLen = len;
}

//-------------------------------------------------------------------------------------------

const tint& AOBase::getROutNo() const
{
	return m_rOutNo;
}

//-------------------------------------------------------------------------------------------

void AOBase::setROutNo(tint no)
{
	m_rOutNo = no;
}

//-------------------------------------------------------------------------------------------

sample_t *AOBase::getRIn(tint ch)
{
	return m_rIn[ch];
}

//-------------------------------------------------------------------------------------------

void AOBase::setRIn(sample_t *in,tint ch)
{
	if(m_rIn[ch]!=0)
	{
        delete [] m_rIn[ch];
	}
	m_rIn[ch] = in;
}

//-------------------------------------------------------------------------------------------

sample_t *AOBase::getROut(tint ch)
{
	return m_rOut[ch];
}

//-------------------------------------------------------------------------------------------

void AOBase::setROut(sample_t *out,tint ch)
{
	if(m_rOut[ch]!=0)
	{
        delete [] m_rOut[ch];
	}
	m_rOut[ch] = out;
}

//-------------------------------------------------------------------------------------------

const tfloat64& AOBase::getROutDrift() const
{
	return m_rOutDrift;
}

//-------------------------------------------------------------------------------------------

void AOBase::setROutDrift(tfloat64 drift)
{
	m_rOutDrift = drift;
}

//-------------------------------------------------------------------------------------------

const bool& AOBase::getRCodecCompleteFlag() const
{
    return m_rCodecCompleteFlag;
}

//-------------------------------------------------------------------------------------------

void AOBase::setRCodecCompleteFlag(bool flag)
{
	m_rCodecCompleteFlag = flag;
}

//-------------------------------------------------------------------------------------------

QList<AOResampleInfo>& AOBase::resampleList()
{
	return m_resampleList;
}

//-------------------------------------------------------------------------------------------

const QList<AOResampleInfo>& AOBase::resampleListConst() const
{
	return m_resampleList;
}

//-------------------------------------------------------------------------------------------

const tint& AOBase::getNoOutChannels() const
{
	return m_noOutChannels;
}

//-------------------------------------------------------------------------------------------

void AOBase::setNoOutChannels(tint noChs)
{
	m_noOutChannels = noChs;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::getPauseTime() const
{
	return m_pauseTime;
}

//-------------------------------------------------------------------------------------------

void AOBase::setPauseTime(const common::TimeStamp& t)
{
	m_pauseTime = t;
}

//-------------------------------------------------------------------------------------------

bool AOBase::getPauseAudioFlag() const
{
	return m_pauseAudioFlag;
}

//-------------------------------------------------------------------------------------------

void AOBase::setPauseAudioFlag(bool f)
{
	m_pauseAudioFlag = f;
}

//-------------------------------------------------------------------------------------------

engine::Codec *AOBase::getCompleteCodec()
{
	return m_completeCodec;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCompleteCodec(engine::Codec *c)
{
	m_completeCodec = c;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AOBase::getCurrentOutTime() const
{
	return m_currentOutTime;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCurrentOutTime(const common::TimeStamp& v)
{
	m_currentOutTime = v;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCurrentPlayTime(const common::TimeStamp& v)
{
	m_currentPlayTime.set(v);
}

//-------------------------------------------------------------------------------------------

void AOBase::setCurrentCallbackTime(const common::TimeStamp& v)
{
	m_currentCallbackTime.set(v);
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AOBase::getCodecTimePositionComplete() const
{
	return m_codecTimePositionComplete;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCodecTimePositionComplete(const common::TimeStamp& t)
{
	m_codecTimePositionComplete = t;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AOBase::getNextCodecTimeLengthComplete() const
{
	return m_nextCodecTimeLengthComplete;
}

//-------------------------------------------------------------------------------------------

void AOBase::setNextCodecTimeLengthComplete(const common::TimeStamp& t)
{
	m_nextCodecTimeLengthComplete = t;
}

//-------------------------------------------------------------------------------------------

tint AOBase::getProgFadeState() const
{
	return m_progFadeState;
}

//-------------------------------------------------------------------------------------------

void AOBase::setProgFadeState(tint state)
{
	m_progFadeState = state;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AOBase::getCrossFadeTimeLen() const
{
	return m_crossFadeTimeLen;
}

//-------------------------------------------------------------------------------------------

void AOBase::setCrossFadeTimeLen(const common::TimeStamp& t)
{
	m_crossFadeTimeLen = t;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AOBase::getAudioStartClock() const
{
	return m_audioStartClock;
}

//-------------------------------------------------------------------------------------------

void AOBase::setAudioStartClock(const common::TimeStamp& t)
{
	m_audioStartClock = t;
}

//-------------------------------------------------------------------------------------------

bool AOBase::getStartNextTrackFlag() const
{
	return m_startNextTrackFlag;
}

//-------------------------------------------------------------------------------------------

void AOBase::setStartNextTrackFlag(bool f)
{
	m_startNextTrackFlag = f;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AOBase::getOutputLatencyTime() const
{
	return m_outputLatencyTime;
}

//-------------------------------------------------------------------------------------------

void AOBase::setOutputLatencyTime(const common::TimeStamp& t)
{
	m_outputLatencyTime = t;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBase::getStopTimeClock() const
{
	common::TimeStamp t = const_cast<const common::TimeStamp &>(m_stopTimeClock);
	return t;
}

//-------------------------------------------------------------------------------------------

void AOBase::setStopTimeClock(const common::TimeStamp& t)
{
	m_stopTimeClock.set(t);
}

//-------------------------------------------------------------------------------------------

bool AOBase::getStopTimeFlag() const
{
	return m_stopTimeFlag;
}

//-------------------------------------------------------------------------------------------

void AOBase::setStopTimeFlag(bool f)
{
	m_stopTimeFlag = f;
}

//-------------------------------------------------------------------------------------------

AOQueryDevice *AOBase::getDeviceInfo()
{
	return m_deviceInfo;
}

//-------------------------------------------------------------------------------------------

void AOBase::setDeviceIndex(int idx)
{
	m_deviceIdx = idx;
}

//-------------------------------------------------------------------------------------------

bool AOBase::canCallSlot(SlotType type)
{
	return canCallSlot(type,reinterpret_cast<void *>(0));
}

//-------------------------------------------------------------------------------------------

bool AOBase::canCallSlot(SlotType type,void *cData)
{
	bool res = m_recursiveSlotList.isEmpty();
	m_recursiveSlotList.append(QPair<SlotType,void *>(type,cData));
	
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::canCallSlot - %d, %d\n", (int)type, (int)res);
#endif
	
	return res;
}

//-------------------------------------------------------------------------------------------

void AOBase::slotComplete()
{
	if(!m_recursiveSlotList.isEmpty())
	{
		m_recursiveSlotList.removeFirst();
		while(!m_recursiveSlotList.isEmpty())
		{
			QPair<SlotType,void *> p = m_recursiveSlotList.takeFirst();

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	common::Log::g_Log.print("AOBase::slotComplete\n");
#endif
			switch(p.first)
			{
				case e_onTimer:
					doTimer();
					break;
				
				case e_onCodecInit:
					doCodecInit(p.second);
					break;
				
				case e_onEventTimer:
					doEventTimer();
					break;
					
				case e_onAudioEvent:
					{
						AudioEvent *pAudioEvent = reinterpret_cast<AudioEvent *>(p.second);
						audioEventMain(pAudioEvent);
						delete pAudioEvent;
					}
					break;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void AOBase::forceBitsPerSample(tint noBits)
{
	m_forceBitsPerSample = noBits;
}

//-------------------------------------------------------------------------------------------

bool AOBase::setCodecSampleFormatType(engine::Codec *codec, engine::RData *item)
{
	return codec->setDataTypeFormat(engine::e_SampleFloat);
}

//-------------------------------------------------------------------------------------------

int AOBase::getNoChannelsMapped()
{
	int noChs = 2;
	m_deviceInfoMutex.lock();
	if(m_deviceIdx >= 0 && m_deviceIdx < m_deviceInfo->noDevices())
	{
		noChs = m_deviceInfo->deviceDirect(m_deviceIdx)->channelMap()->noMappedChannels();
	}
	m_deviceInfoMutex.unlock();
	return noChs;
}

//-------------------------------------------------------------------------------------------

bool AOBase::setupDSDCodecForPlayback(QSharedPointer<AOQueryDevice::Device> pDevice)
{
	bool res = false;
	engine::dsd::DSDCodec *dsdCodec = dynamic_cast<engine::dsd::DSDCodec *>(getCodec());
	
	if(dsdCodec != NULL)
	{
		if(pDevice->isDSDNative() && pDevice->isDSDFrequencySupported(getCodec()->frequency()))
		{
			res = true;
		}
		else if(dsdCodec->dataTypesSupported() & engine::e_SampleFloat)
		{
			int cFreq = 352800;
			
			if(!pDevice->isFrequencySupported(cFreq))
			{
				if(pDevice->isFrequencySupported(176400))
				{
					cFreq = 176400;
				}
				else if(pDevice->isFrequencySupported(88200))
				{
					cFreq = 88200;
				}
			}
			res = dsdCodec->setOutputPCM(cFreq);
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOBase::canAudioFromCodecBePlayed()
{
	bool res = true;
	
	if((getCodec()->dataTypesSupported() & (engine::e_SampleDSD8LSB | engine::e_SampleDSD8MSB)) != 0)
	{
		QSharedPointer<AOQueryDevice::Device> pDevice = getCurrentDevice();
		res = setupDSDCodecForPlayback(pDevice);
	}
	return res;
}

//-------------------------------------------------------------------------------------------
#if defined(DEBUG_LOG_AUDIOOUTPUT)
//-------------------------------------------------------------------------------------------

void AOBase::appendDebugLog(const AODebugItem& item)
{
	AODebugItem *aItem,*bItem,*cItem,*nItem = new AODebugItem(item);
	
	aItem = 0;
	bItem = (AODebugItem *)(m_debugList);
	cItem = (AODebugItem *)(m_debugList);
	while(bItem!=0)
	{
		if(bItem->isMarked())
		{
			if(aItem!=0)
			{
				aItem->setNext(0);
			}
			else
			{
				cItem = 0;
			}
			break;
		}
		aItem = bItem;
		bItem = bItem->next();
	}
	nItem->setNext(cItem);
	m_debugList = nItem;
}

//-------------------------------------------------------------------------------------------

AODebugItem *AOBase::getDebugLog()
{
	AODebugItem *item;
	bool loop = true;
	
	while(loop)
	{
		if(!(m_mutexCount & 1))
		{
			item = (AODebugItem *)(m_debugList);
			if(item!=0)
			{
				item->mark();
			}
			loop = false;
		}
	}
	return item;
}

//-------------------------------------------------------------------------------------------

void AOBase::printDebugLog()
{
	AODebugItem *item = getDebugLog();
	
	if(item!=0)
	{
		QList<AODebugItem *> list;
		
		while(item!=0)
		{
			list.prepend(item);
			item = item->next();
		}
		while(list.size()>0)
		{
			QString tStr;
			
			item = list.takeFirst();
			
			switch(item->type())
			{
				case AODebugItem::e_clockSkew:
					tStr = "Clock Skew";
					break;
				case AODebugItem::e_referenceTimeStart:
					tStr = "Reference Start";
					break;
				case AODebugItem::e_referenceFill:
					tStr = "Reference Fill";
					break;
				case AODebugItem::e_partOutputA:
					tStr = "Output A";
					break;
				case AODebugItem::e_partOutputB:
					tStr = "Output B";
					break;
				case AODebugItem::e_partOutputC:
					tStr = "Output C";
					break;
				case AODebugItem::e_partOutputD:
					tStr = "Output D";
					break;
				case AODebugItem::e_silenceOutputA:
					tStr = "Silence A";
					break;
				case AODebugItem::e_silenceOutputB:
					tStr = "Silence B";
					break;
			}
			
			common::Log::g_Log.print("%s,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%d,%d,%d\n",
			        tStr.toLatin1().constData(),
			        static_cast<tfloat64>(item->nowClock()),
			        static_cast<tfloat64>(item->referenceClock()),
			        static_cast<tfloat64>(item->audioClock()),
			        static_cast<tfloat64>(item->playTime()),
			        static_cast<tfloat64>(item->outTime()),
			        static_cast<tfloat64>(item->audioStartClock()),
			        static_cast<tfloat64>(item->timeA()),
			        static_cast<tfloat64>(item->timeB()),
			        static_cast<tfloat64>(item->timeC()),	
					item->amount(),
			        item->offset(),
			        item->total());
		}
	}
}

//-------------------------------------------------------------------------------------------
// AODebugItem
//-------------------------------------------------------------------------------------------

AODebugItem::AODebugItem(Type t) : m_next(0),
	m_marker(false),
	m_type(t),
	m_nowClock(),
	m_referenceClock(),
	m_audioClock(),
	m_playTime(),
	m_outTime(),
	m_audioStartClock(),
	m_timeA(),
	m_timeB(),
	m_timeC(),
	m_amount(0),
	m_offset(0),
	m_total(0)
{}

//-------------------------------------------------------------------------------------------

AODebugItem::AODebugItem(const AODebugItem& rhs) : m_next(0),
	m_marker(false),
	m_type(AODebugItem::e_clockSkew),
	m_nowClock(),
	m_referenceClock(),
	m_audioClock(),
	m_playTime(),
	m_outTime(),
	m_audioStartClock(),
	m_timeA(),
	m_timeB(),
	m_timeC(),
	m_amount(0),
	m_offset(0),
	m_total(0)
{
	copy(rhs);
}

//-------------------------------------------------------------------------------------------

const AODebugItem& AODebugItem::operator = (const AODebugItem& rhs)
{
	if(this!=&rhs)
	{
		copy(rhs);
	}
	return *this;
}

//-------------------------------------------------------------------------------------------

void AODebugItem::copy(const AODebugItem& rhs)
{
	m_type = rhs.m_type;
	m_nowClock = rhs.m_nowClock;
	m_referenceClock = rhs.m_referenceClock;
	m_audioClock = rhs.m_audioClock;
	m_playTime = rhs.m_playTime;
	m_outTime = rhs.m_outTime;
	m_audioStartClock = rhs.m_audioStartClock;
	m_timeA = rhs.m_timeA;
	m_timeB = rhs.m_timeB;
	m_timeC = rhs.m_timeC;
	m_amount = rhs.m_amount;
	m_offset = rhs.m_offset;
	m_total = rhs.m_total;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
} //namespace audioio
} //namespace omega
//-------------------------------------------------------------------------------------------
