//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOWIN32_H
#define __OMEGA_AUDIOIO_AOWIN32_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOBase.h"
#include "audioio/inc/ASIODriver.h"
#include "audioio/inc/WasAPIIF.h"
#include "audioio/inc/SampleConverter.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOWin32 : public AOBase
{
	public:
		Q_OBJECT
		
	public:
		AOWin32(QObject *parent = 0);
		virtual ~AOWin32();

		static bool startAudioService();
		static void stopAudioService();

	protected:
		
		static AOWin32 *m_instance;

		AOQueryDevice::Device::Type m_deviceType;

		// ------ ASIO ------

		// The ASIO driver interface.
		ASIODriver *m_driver;
		
		// Minimum, preferred and maximum buffer sizes.
		tint m_bufferSizeMin;
		tint m_bufferSizePref;
		tint m_bufferSizeMax;
		
		// ASIO driver information
		ASIODriverInfo m_driverInfo;
		// ASIO buffer information
		ASIOBufferInfo m_bufferInfos[c_kMaxOutputChannels];
		// ASIO channel information
		ASIOChannelInfo m_channelInfos[c_kMaxOutputChannels];
		// ASIO callback functions
		ASIOCallbacks m_driverCallbacks;
		// Flag true when ASIOStart() is called, false on ASIOStop()
		bool m_driverRunFlag;
		// Does ASIO driver support ASIOOutputReady() callback.
		bool m_driverPostOutput;
		
		// This is set to true on gettings a kAsioResetRequest message.
		volatile bool m_resetAudioFlag;
		
		// This is set to true on getting a kASIOLatenciesChanged message.
		volatile bool m_latencyChangeFlag;
		
		// ------ WasAPI ------
		
		WasAPIDeviceSPtr m_pWASDevice;
		IAudioClientIFSPtr m_pAudioClient;
		IAudioRenderClientIFSPtr m_pAudioRenderClient;
		IAudioClockIFSPtr m_pAudioClock;
		WAVEFORMATEX *m_pWASFormat;
		UINT32 m_bufferWASFrameCount;
		HANDLE m_hWASThread;
		HANDLE m_hWASEvent;
		HANDLE m_hWASFeedbackEvent;
		QMutex m_wasMutex;
		QSharedPointer<SampleConverter> m_pSampleConverter;
		volatile bool m_wasRunThread;
		bool m_wasRunFlag;
		bool m_wasPlayExclusive;
		bool m_isVolumeDevice;
		
		virtual void printError(const tchar *strE,const tchar *strR) const;
		
		virtual engine::AData *allocateData(tint len,tint inChannel,tint outChannel);
		
		virtual bool openAudio();
		virtual void closeAudio();
		virtual bool startAudioDevice();
		virtual void stopAudioDevice();
		virtual bool isAudio() const;
		
		virtual bool openAudioASIO();
		virtual void closeAudioASIO();
		virtual bool startAudioDeviceASIO();
		virtual void stopAudioDeviceASIO();
		virtual bool isAudioASIO() const;

		virtual bool openAudioWasAPI();
		virtual bool openAudioWasAPIImpl();
		virtual void closeAudioWasAPI();
		virtual bool startAudioDeviceWasAPI();
		virtual void stopAudioDeviceWasAPI();
		virtual bool isAudioWasAPI() const;

		virtual bool openAudioFrequency();
		virtual bool createAudioBuffers();

		virtual ASIODriver *getASIODriver();

		static void sampleRateChanged(ASIOSampleRate sRate);
		virtual void sampleRateChangedI(ASIOSampleRate sRate);
		
		static long asioMessages(long selector,long value,void *message,double *opt);
		virtual long asioMessagesI(long selector,long value,void *message,double *opt);
		
		static void bufferSwitch(long index,ASIOBool processNow);
		virtual void bufferSwitchI(long index,ASIOBool processNow);
		
		static ASIOTime *bufferSwitchTimeInfo(ASIOTime *timeInfo,long index,ASIOBool processNow);
		virtual ASIOTime *bufferSwitchTimeInfoI(ASIOTime *timeInfo,long index,ASIOBool processNow);

		virtual common::TimeStamp getTimeFromASIO(const ASIOTimeStamp *tS) const;
		virtual void getTimeFromOmega(const common::TimeStamp& t,ASIOTimeStamp *tS) const;
		
		bool isDSDAudio();
		bool openAudioDSDFrequency(const AOQueryDevice::Device& dev);

		virtual void processMessages();
		virtual void processStopTimeMessage();
		virtual void processMessagesASIO();
		virtual void processMessagesWasAPI();
		
		virtual IOTimeStamp createIOTimeStamp(const ASIOTime *sysTime) const;
		virtual void processDataForOutput(engine::RData *data);
		
		virtual void writeToAudioPostProcess();
		virtual void writeToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,
                                                          const engine::RData *data,
                                                          tint partNumber,
                                                          tint inChannelIndex,
                                                          tint bufferIndex,
                                                          tint outChannelIndex,
                                                          tint inputSampleIndex,
                                                          tint outputSampleIndex,
                                                          tint amount);



		virtual void writeToAudioOutputBufferFromPartDataASIO(AbstractAudioHardwareBuffer *pBuffer,
                                                          const engine::RData *data,
                                                          tint partNumber,
                                                          tint inChannelIndex,
                                                          tint bufferIndex,
                                                          tint outChannelIndex,
                                                          tint inputSampleIndex,
                                                          tint outputSampleIndex,
                                                          tint amount);

		virtual void writeToAudioOutputBufferFromPartDataWasAPI(AbstractAudioHardwareBuffer *pBuffer,
                                                          const engine::RData *data,
                                                          tint partNumber,
                                                          tint inChannelIndex,
                                                          tint bufferIndex,
                                                          tint outChannelIndex,
                                                          tint inputSampleIndex,
                                                          tint outputSampleIndex,
                                                          tint amount);

		
		virtual QSharedPointer<AOQueryDevice::Device> copyDeviceInformation(const AOQueryDevice::Device& iDevice);
		
		virtual bool activateWasAPIAudioDevice(WAVEFORMATEX *pFormat);
		virtual QSharedPointer<SampleConverter> createWASSampleConverter(WAVEFORMATEX *pFormat);
		virtual IOTimeStamp createIOTimeStampWasAPI() const;

		static DWORD WINAPI writeWASAudioThread(LPVOID arg);
		virtual void writeWASAudioThreadImpl();
		virtual void writeWASAudio();

		virtual bool setCodecSampleFormatType(engine::Codec *codec, engine::RData *item);
		
		virtual REFERENCE_TIME alignedBufferDuration(WAVEFORMATEX* pFormat);

		static void onVolumeChangeNotification(LPVOID pVInstance, sample_t vol);
		virtual void onVolumeChangeNotification(sample_t vol);

		virtual bool isASIODevice();
		virtual bool isWasAPIDevice();
		
		virtual void openAudioWasAPIVolume();
		virtual void closeAudioWasAPIVolume();
		virtual void doSetVolume(sample_t vol, bool isCallback);
		
	protected slots:
	
		virtual void onStop();
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

