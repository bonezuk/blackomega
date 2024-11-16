//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_WASAPILAYERIF_H
#define __OMEGA_AUDIOIO_WASAPILAYERIF_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/WasAPIIF.h"
#include "audioio/inc/WasAPICOMInterfaces.h"
#include "audioio/inc/FormatDescriptionUtils.h"
#include "common/inc/Log.h"
#include "audioio/inc/WasAPIVolumeEvents.h"

#include <QSettings>
#include <QSet>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class WasAPIDeviceLayer;

//-------------------------------------------------------------------------------------------

#define WAVE_EXT_16    0x0001
#define WAVE_BASIC_16  0x0002
#define WAVE_EXT_24    0x0004
#define WAVE_BASIC_24  0x0008
#define WAVE_EXT_32    0x0010
#define WAVE_BASIC_32  0x0020
#define WAVE_FLOAT_32  0x0040

//-------------------------------------------------------------------------------------------

#define NUMBER_WASAPI_MAXCHANNELS NUMBER_MAXCHANNELS
#define NUMBER_WASAPI_MAXBITS 6
#define NUMBER_WASAPI_MAXFREQUENCIES NUMBER_MAXFREQUENCIES

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT WasAPILayerIF : public WasAPIIF
{
	public:
		Q_OBJECT
	
	public:
		WasAPILayerIF();
		virtual ~WasAPILayerIF();
		
		virtual QStringList enumerateDeviceIds();
		virtual QSharedPointer<WasAPIDevice> getDevice(const QString& devID);

		virtual IMMDeviceEnumeratorIFSPtr enumerator();

	protected:
	
		IMMDeviceEnumeratorIFSPtr m_pEnumerator;
		
		virtual bool init();
		virtual void done();
		
		virtual IMMDeviceCollectionIFSPtr createDeviceCollectionIF(IMMDeviceCollection *pCollection);
		virtual IMMDeviceIFSPtr createDeviceIF(IMMDevice *pDevice);
		virtual WasAPIDeviceLayer *createDeviceInstance();
};

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT WasAPIDeviceLayer : public WasAPIDevice
{
	public:
		Q_OBJECT
	
	public:
	
		friend class WasAPILayerIF;

		typedef enum
		{
			e_formatPCM = WAVE_FORMAT_PCM,
			e_formatFloat = WAVE_FORMAT_IEEE_FLOAT,
			e_formatExtensible = WAVE_FORMAT_EXTENSIBLE			
		} FormatType;
		
		typedef enum
		{
			e_8Bit = 0,
			e_16Bit_LittleEndian,
			e_24Bit_LittleEndian,
			e_32Bit_LittleEndian,
			e_Float_LittleEndian,
			e_16Bit_BigEndian,
			e_24Bit_BigEndian,
			e_32Bit_BigEndian,
			e_Float_BigEndian,
			e_WaveUnknown
		} WaveType;
		
		typedef void (*ProcessPartFunc)(WasAPIDeviceLayer *pDeviceAPI,IPartIFSPtr pPart,void *pValue);
		
	public:
		WasAPIDeviceLayer();
		virtual ~WasAPIDeviceLayer();

		virtual QString id() const;
		virtual QString name() const;

		virtual bool isExclusive() const;
		virtual void setExclusive(bool flag);

		virtual QSet<int> queryFrequencyCapabilities(AccessModeSharedDevice accessMode = e_Settings);
		virtual int queryChannelCapabilities(AccessModeSharedDevice accessMode = e_Settings);

		virtual int getNumberOfBitsFromIndex(int idx) const;
		virtual int getNumberOfChannelsFromIndex(int idx) const;
		virtual int getFrequencyFromIndex(int idx) const;

		virtual bool isFormat(int chIdx,int bitIdx,int freqIdx) const;
		
		virtual WAVEFORMATEX *findClosestSupportedFormat(const FormatDescription& sourceDesc);
		
		virtual IAudioClientIFSPtr getAudioClient();
		virtual void releaseAudioClient();

		static void printWaveFormat(WAVEFORMATEX *pFormat);
		void printIndexedFormatSupport(tint bitIdx,tint chIdx,tint freqIdx);
		QString capabilityCSVIndexed(tint bitIdx, tint chIdx, tint freqIdx, tint isExculsive);
		
		virtual bool isDeviceVolume();
		virtual sample_t getVolume();
		virtual bool setVolume(sample_t vol);

		virtual bool setupVolumeNotification(VolumeChangeNotifier pNotifier, LPVOID pVInstance);
		virtual void shutdownVolumeNotification();
	
	protected:
		
		IMMDeviceIFSPtr m_pDevice;
		IAudioClientIFSPtr m_pAudioClient;
		
		ISimpleAudioVolumeIFSPtr m_pVolumeShared;
		IAudioEndpointVolumeIFSPtr m_pVolumeExclusive;
		
		IAudioSessionControlIFSPtr m_pAudioSessionControl;
		WasAPISharedVolumeEvents *m_pVolumeEventsShared;
		WasAPIExclusiveVolumeEvents *m_pVolumeEventsExclusive;
		
		tint m_formatsShared[NUMBER_WASAPI_MAXCHANNELS][NUMBER_WASAPI_MAXBITS][NUMBER_WASAPI_MAXFREQUENCIES];
		tint m_formatsExclusive[NUMBER_WASAPI_MAXCHANNELS][NUMBER_WASAPI_MAXBITS][NUMBER_WASAPI_MAXFREQUENCIES];
		
		virtual bool init(const QString& devID);
		virtual void done();
		
		virtual void blank();
		virtual void blank(bool exclusive);

		virtual IMMDeviceEnumeratorIFSPtr deviceEnumerator();
		virtual IMMDeviceIFSPtr createDeviceIF(IMMDevice *pDevice);
		virtual IAudioClientIFSPtr createAudioClientIF(IAudioClient *pAudioClient);
		virtual IPropertyStoreIFSPtr createPropertyStoreIF(IPropertyStore *pPropertyStore) const;

		virtual void defaultWaveFormat(WAVEFORMATEX& format) const;
		virtual void defaultWaveExtensibleFormat(WAVEFORMATEXTENSIBLE& format) const;
		virtual void defaultWaveExtensibleFloatFormat(WAVEFORMATEXTENSIBLE& format, bool is64Bit) const;
		
		virtual void setWaveFormat(int noChannels, int noBits, int frequency, WAVEFORMATEX& format) const;
		virtual void setWaveExtensibleFormat(int noChannels, int noBits, int frequency, WAVEFORMATEXTENSIBLE& format) const;
		virtual void setWaveExtensibleFormat(int noChannels, int noBits, int noBytes, int frequency, WAVEFORMATEXTENSIBLE& format) const;
		virtual void setWaveExtensibleFloatFormat(int noChannels,int frequency, bool is64Bit, WAVEFORMATEXTENSIBLE& format) const;
		
		DWORD defaultChannelMask(int noChannels) const;

		virtual WAVEFORMATEXTENSIBLE *toWaveExtensible(WAVEFORMATEX *pFormat) const;
		
		virtual bool isFormatSupported(WAVEFORMATEX *pFormat);

		virtual int getIndexOfBits(const WAVEFORMATEX *pFormat) const;
		virtual int getIndexOfChannels(const WAVEFORMATEX *pFormat) const;
		virtual int getIndexOfFrequency(const WAVEFORMATEX *pFormat) const;
		
		virtual tint getNoOfBitsFromWaveFormat(const WAVEFORMATEX *pFormat) const;
		virtual tint getNoOfChannelsFromWaveFormat(const WAVEFORMATEX *pFormat) const;
		virtual tint getFrequencyFromWaveFormat(const WAVEFORMATEX *pFormat) const;
		
		virtual bool loadFormats();
		virtual bool loadFormats(bool exclusive);
		virtual bool saveFormats();
		virtual bool saveFormats(bool exclusive);
		virtual QString settingsKey(bool exclusive) const;
		virtual QString settingsValidKey(bool exclusive) const;
		
		virtual void setFrequencyInWaveFormat(int frequency,WAVEFORMATEX *pFormat) const;
		virtual void setChannelsInWaveFormat(int channel,WAVEFORMATEX *pFormat) const;
		
		virtual void queryDeviceFormatCapabilities();
		virtual bool hasIndexedFormatWithUpdate(tint bitIdx,tint chIdx,tint freqIdx, bool exculsive);
		
		virtual void setWaveFormatFromIndex(tint bitIdx, tint chIdx, tint freqIdx, WAVEFORMATEX& format);
		virtual void setWaveExtensibleFormatFromIndex(tint bitIdx, tint chIdx, tint freqIdx, WAVEFORMATEXTENSIBLE& format);
		virtual void setWaveExtensibleFormatFromIndex(tint bitIdx, tint chIdx, tint freqIdx, bool is32Bit, WAVEFORMATEXTENSIBLE& format);
		virtual void setWaveExtensibleFloatFormatFromIndex(tint chIdx, tint freqIdx, bool is64Bit, WAVEFORMATEXTENSIBLE& format);
		
		virtual void populateFormatsSupported(FormatsSupported& support);
		virtual FormatDescription descriptionFromWaveFormat(const WAVEFORMATEX *pFormat);
		virtual WAVEFORMATEX *descriptionToWaveFormat(const FormatDescription& desc);
		
		virtual bool findClosestDescription(const FormatDescription& sourceDesc, FormatDescription& descClosest);
		
		static QString waveFormatTypeName(WORD wFormatTag);
		static QString waveFormatGUIDType(GUID guid);
		static QString guidString(GUID id);
		static QString printChannelMask(DWORD dwChannelMask);
		
		virtual void printIsFormatSupported(WAVEFORMATEX *pFormat, bool isExcl);
		
		virtual WAVEFORMATEX *waveFormatFromType(tint noChannels, tint noBits, tint frequency, tint type) const;
		virtual int queryFormatIndexCapability(tint bitIdx, tint chIdx, tint freqIdx, tint isExculsive);

		virtual int bitIndexForFDIndex(int nativeBitIdx) const;
		virtual int nativeBitIndexFromFDIndex(int fdBitIndex) const;
		
		virtual WAVEFORMATEX *supportedWaveFormatFromDescription(const FormatDescription& desc);
		
		virtual ISimpleAudioVolumeIFSPtr getVolumeSharedIF();
		virtual IAudioEndpointVolumeIFSPtr getVolumeExclusiveIF();
		virtual void releaseVolumeIF();
		
		virtual bool isDeviceVolumeExclusive();
		virtual bool isDeviceVolumeShared();
		virtual sample_t getVolumeShared();
		virtual sample_t getVolumeExclusive();
		virtual bool setVolumeShared(sample_t vol);
		virtual bool setVolumeExclusive(sample_t vol);

		virtual bool setupVolumeNotificationShared(VolumeChangeNotifier pNotifier, LPVOID pVInstance);
		virtual bool setupVolumeNotificationExclusive(VolumeChangeNotifier pNotifier, LPVOID pVInstance);
		
		virtual bool isExclusiveFromAM(AccessModeSharedDevice accessMode) const;
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

