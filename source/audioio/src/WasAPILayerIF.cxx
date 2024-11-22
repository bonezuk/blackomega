#include "audioio/inc/WasAPILayerIF.h"
#include "audioio/inc/AudioSettings.h"
#include "engine/inc/FormatType.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(WasAPIIFFactory,WasAPIIF, \
                            WasAPILayerIFFactory,WasAPILayerIF, \
                            "wasapi",false)

//-------------------------------------------------------------------------------------------

WasAPILayerIF::WasAPILayerIF() : WasAPIIF(),
	m_pEnumerator()
{}

//-------------------------------------------------------------------------------------------

WasAPILayerIF::~WasAPILayerIF()
{}

//-------------------------------------------------------------------------------------------

bool WasAPILayerIF::init()
{
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

	IMMDeviceEnumerator *pEnumerator;
	HRESULT hr;
	bool res = false;
	
	done();
	
	hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, reinterpret_cast<void **>(&pEnumerator));
	if(hr==S_OK && pEnumerator!=0)
	{
		IMMDeviceEnumeratorIFSPtr pEnum(new IMMDeviceEnumeratorIF(pEnumerator));
		m_pEnumerator = pEnum;
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void WasAPILayerIF::done()
{
	if(!m_pEnumerator.isNull())
	{
		IMMDeviceEnumeratorIFSPtr pEmpty;
		m_pEnumerator = pEmpty;
	}
}

//-------------------------------------------------------------------------------------------

IMMDeviceCollectionIFSPtr WasAPILayerIF::createDeviceCollectionIF(IMMDeviceCollection *pCollection)
{
	return IMMDeviceCollectionIFSPtr(new IMMDeviceCollectionIF(pCollection));
}

//-------------------------------------------------------------------------------------------

IMMDeviceIFSPtr WasAPILayerIF::createDeviceIF(IMMDevice *pDevice)
{
	return IMMDeviceIFSPtr(new IMMDeviceIF(pDevice));
}

//-------------------------------------------------------------------------------------------

IPropertyStoreIFSPtr WasAPIDeviceLayer::createPropertyStoreIF(IPropertyStore *pPropertyStore) const
{
	IPropertyStoreIFSPtr pPropertyStoreIF(new IPropertyStoreIF(pPropertyStore));
	return pPropertyStoreIF;
}

//-------------------------------------------------------------------------------------------

QStringList WasAPILayerIF::enumerateDeviceIds()
{
	QStringList deviceList;
	IMMDeviceCollection *pCollection = 0;
	HRESULT hr;
	
	hr = m_pEnumerator->EnumAudioEndpoints(eRender,DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED,&pCollection);
	if(hr==S_OK && pCollection!=0)
	{
		UINT len = 0;
		IMMDeviceCollectionIFSPtr pCollectionIF = createDeviceCollectionIF(pCollection);
		
		hr = pCollectionIF->GetCount(&len);
		if(hr==S_OK)
		{
			for(UINT deviceNo=0;deviceNo<len;deviceNo++)
			{
				IMMDevice *pDevice = 0;
				
				hr = pCollectionIF->Item(deviceNo,&pDevice);
				if(hr==S_OK && pDevice!=0)
				{
					LPWSTR pDeviceName;
					IMMDeviceIFSPtr pDeviceIF = createDeviceIF(pDevice);
					
					hr = pDeviceIF->GetId(&pDeviceName);
					if(hr==S_OK && pDeviceName!=0)
					{
						QString name = QString::fromUtf16(reinterpret_cast<const char16_t *>(pDeviceName));
						deviceList.append(name);
						CoTaskMemFree(pDeviceName);
					}
				}
			}
		}
	}
	return deviceList;
}

//-------------------------------------------------------------------------------------------

IMMDeviceEnumeratorIFSPtr WasAPILayerIF::enumerator()
{
	return m_pEnumerator;
}

//-------------------------------------------------------------------------------------------

WasAPIDeviceLayer *WasAPILayerIF::createDeviceInstance()
{
	return new WasAPIDeviceLayer();
}

//-------------------------------------------------------------------------------------------

QSharedPointer<WasAPIDevice> WasAPILayerIF::getDevice(const QString& devID)
{
	WasAPIDeviceSPtr pDevice;

	if(!devID.isEmpty())
	{
		WasAPIDeviceLayer *pNDevice = createDeviceInstance();
		
		if(pNDevice->init(devID))
		{
			WasAPIDeviceSPtr pDev(pNDevice);
			pDevice = pDev;
		}
		else
		{
			delete pNDevice;
		}
	}
	return pDevice;
}

//-------------------------------------------------------------------------------------------
// WasAPIDeviceLayer
//-------------------------------------------------------------------------------------------

WasAPIDeviceLayer::WasAPIDeviceLayer() : WasAPIDevice(),
	m_pDevice(),
	m_pAudioClient(),
	m_pVolumeShared(),
	m_pVolumeExclusive(),
	m_pAudioSessionControl(),
	m_pVolumeEventsShared(0),
	m_pVolumeEventsExclusive(0)
{
	WasAPIDeviceLayer::blank();
}

//-------------------------------------------------------------------------------------------

WasAPIDeviceLayer::~WasAPIDeviceLayer()
{
	WasAPIDeviceLayer::done();
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::blank()
{
	blank(true);
	blank(false);
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::blank(bool exclusive)
{
	for(int i = 0; i < NUMBER_WASAPI_MAXCHANNELS; i++)
	{
		for(int j = 0; j < NUMBER_WASAPI_MAXBITS; j++)
		{
			for(int k = 0; k < NUMBER_WASAPI_MAXFREQUENCIES; k++)
			{
				if(exclusive)
				{
					m_formatsExclusive[i][j][k] = -1;
				}
				else
				{
					m_formatsShared[i][j][k] = -1;
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

IMMDeviceEnumeratorIFSPtr WasAPIDeviceLayer::deviceEnumerator()
{
	QSharedPointer<WasAPILayerIF> pLayer = WasAPIIF::instance().dynamicCast<WasAPILayerIF>();
	return pLayer->enumerator();
}

//-------------------------------------------------------------------------------------------

IMMDeviceIFSPtr WasAPIDeviceLayer::createDeviceIF(IMMDevice *pDevice)
{
	IMMDeviceIFSPtr pDeviceIF(new IMMDeviceIF(pDevice));
	return pDeviceIF;
}

//-------------------------------------------------------------------------------------------

IAudioClientIFSPtr WasAPIDeviceLayer::createAudioClientIF(IAudioClient *pAudioClient)
{
	IAudioClientIFSPtr pAudioClientIF(new IAudioClientIF(pAudioClient));
	return pAudioClientIF;
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::init(const QString& devID)
{
	HRESULT hr;
	LPWSTR pDeviceName;
	IMMDevice *pDevice = 0;
	bool res = false;
	
	done();

	if(!devID.isEmpty())
	{
		pDeviceName = reinterpret_cast<LPWSTR>(CoTaskMemAlloc((devID.length() + 1) * sizeof(tuint16)));
		if(pDeviceName!=0)
		{
			memcpy(pDeviceName,devID.utf16(),devID.length() * sizeof(tuint16));
			pDeviceName[devID.length()] = 0;
			
			hr = deviceEnumerator()->GetDevice(pDeviceName,&pDevice);
			if(hr==S_OK && pDevice!=0)
			{
				IAudioClientIFSPtr pAudioClient;

				m_pDevice = createDeviceIF(pDevice);
				pAudioClient = getAudioClient();
				if(!pAudioClient.isNull())
				{
					if(!loadFormats())
					{
						blank();
					}
					queryDeviceFormatCapabilities();
					releaseAudioClient();
					res = true;
				}
			}
			CoTaskMemFree(pDeviceName);
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::done()
{
	shutdownVolumeNotification();
	releaseVolumeIF();
	if(!m_pDevice.isNull())
	{
		if(!WasAPIIF::instance().dynamicCast<WasAPILayerIF>().isNull())
		{
			saveFormats();
		}
	}
	m_pAudioSessionControl.clear();
	m_pAudioClient.clear();
	m_pDevice.clear();
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::defaultWaveFormat(WAVEFORMATEX& format) const
{
	memset(&format,0,sizeof(WAVEFORMATEX));
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 2;
	format.nSamplesPerSec = 44100;
	format.nAvgBytesPerSec = 44100 * 2 * 2;
	format.nBlockAlign = 4;
	format.wBitsPerSample = 16;
	format.cbSize = 0;
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::defaultWaveExtensibleFormat(WAVEFORMATEXTENSIBLE& format) const
{
	memset(&format,0,sizeof(WAVEFORMATEXTENSIBLE));
	format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	format.Format.nChannels = 2;
	format.Format.nSamplesPerSec = 44100;
	format.Format.nAvgBytesPerSec = 44100 * 2 * 2;
	format.Format.nBlockAlign = 4;
	format.Format.wBitsPerSample = 16;
	format.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
	format.Samples.wValidBitsPerSample = 16;
	format.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
	format.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::defaultWaveExtensibleFloatFormat(WAVEFORMATEXTENSIBLE& format, bool is64Bit) const
{
	int sampleSize = (is64Bit) ? sizeof(tfloat64) : sizeof(tfloat32);
	memset(&format,0,sizeof(WAVEFORMATEXTENSIBLE));
	format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	format.Format.nChannels = 2;
	format.Format.nSamplesPerSec = 44100;
	format.Format.nAvgBytesPerSec = 44100 * 2 * sampleSize;
	format.Format.nBlockAlign = sampleSize;
	format.Format.wBitsPerSample = sampleSize * 8;
	format.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
	format.Samples.wValidBitsPerSample = sampleSize * 8;
	format.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
	format.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::setWaveFormat(int noChannels, int noBits, int frequency, WAVEFORMATEX& format) const
{
	int noBytes = noBits >> 3;
	format.nChannels = noChannels;
	format.nSamplesPerSec = frequency;
	format.nAvgBytesPerSec = frequency * noBytes * noChannels;
	format.nBlockAlign = noBytes * noChannels;
	format.wBitsPerSample = noBits;
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::setWaveExtensibleFormat(int noChannels, int noBits, int frequency, WAVEFORMATEXTENSIBLE& format) const
{
	int noBytes;
	
	noBytes = noBits >> 3;
	if(noBits & 0x7)
		noBytes++;
		
	setWaveExtensibleFormat(noChannels, noBits, noBytes, frequency, format);
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::setWaveExtensibleFormat(int noChannels, int noBits, int noBytes, int frequency, WAVEFORMATEXTENSIBLE& format) const
{
	format.Format.nChannels = noChannels;
	format.Format.nSamplesPerSec = frequency;
	format.Format.nAvgBytesPerSec = frequency * noBytes * noChannels;
	format.Format.nBlockAlign = noBytes * noChannels;
	format.Format.wBitsPerSample = noBytes << 3;
	format.Samples.wValidBitsPerSample = noBits;
	format.dwChannelMask = defaultChannelMask(noChannels);
	format.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;	
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::setWaveExtensibleFloatFormat(int noChannels,int frequency, bool is64Bit,WAVEFORMATEXTENSIBLE& format) const
{
	int noBytes = (is64Bit) ? sizeof(tfloat64) : sizeof(tfloat32);
	
	format.Format.nChannels = noChannels;
	format.Format.nSamplesPerSec = frequency;
	format.Format.nAvgBytesPerSec = frequency * noBytes * noChannels;
	format.Format.nBlockAlign = noBytes * noChannels;
	format.Format.wBitsPerSample = noBytes << 3;	
	format.Samples.wValidBitsPerSample = noBytes << 3;
	format.dwChannelMask = defaultChannelMask(noChannels);
	format.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
}

//-------------------------------------------------------------------------------------------

DWORD WasAPIDeviceLayer::defaultChannelMask(int noChannels) const
{
	DWORD dwChannelMask;
	
	switch(noChannels)
	{
		case 1:
			dwChannelMask = KSAUDIO_SPEAKER_MONO;
			break;
		case 3:
			dwChannelMask = KSAUDIO_SPEAKER_STEREO | SPEAKER_FRONT_CENTER;
			break;
		case 4:
			dwChannelMask = KSAUDIO_SPEAKER_QUAD;
			break;
		case 5:
			dwChannelMask = KSAUDIO_SPEAKER_5POINT1;
			dwChannelMask &= ~SPEAKER_LOW_FREQUENCY;
			break;
		case 6:
			dwChannelMask = KSAUDIO_SPEAKER_5POINT1;
			break;
		case 7:
			dwChannelMask = KSAUDIO_SPEAKER_7POINT0;
			dwChannelMask &= ~SPEAKER_LOW_FREQUENCY;
			break;
		case 8:
			dwChannelMask = KSAUDIO_SPEAKER_7POINT0 | SPEAKER_LOW_FREQUENCY;
			break;
		case 2:
		default:
			dwChannelMask = KSAUDIO_SPEAKER_STEREO;
			break;
	}
	return dwChannelMask;
}

//-------------------------------------------------------------------------------------------

WAVEFORMATEXTENSIBLE *WasAPIDeviceLayer::toWaveExtensible(WAVEFORMATEX *pFormat) const
{
	WAVEFORMATEXTENSIBLE *pExFormat = new WAVEFORMATEXTENSIBLE;
	
	memset(pExFormat,0,sizeof(WAVEFORMATEXTENSIBLE));
	pExFormat->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	pExFormat->Format.nChannels = pFormat->nChannels;
	pExFormat->Format.nSamplesPerSec = pFormat->nSamplesPerSec;
	pExFormat->Format.nAvgBytesPerSec = pFormat->nAvgBytesPerSec;
	pExFormat->Format.nBlockAlign = pFormat->nBlockAlign;
	pExFormat->Format.wBitsPerSample = pFormat->wBitsPerSample;
	pExFormat->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
	pExFormat->Samples.wValidBitsPerSample = pFormat->wBitsPerSample;
	pExFormat->dwChannelMask = defaultChannelMask(pFormat->nChannels);
	pExFormat->SubFormat = (pFormat->wFormatTag==WAVE_FORMAT_IEEE_FLOAT) ? KSDATAFORMAT_SUBTYPE_IEEE_FLOAT : KSDATAFORMAT_SUBTYPE_PCM;	
	return pExFormat;
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::isFormatSupported(WAVEFORMATEX *pFormat)
{
	HRESULT hr;
	
	if(isExclusive())
	{
		hr = m_pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE,pFormat,0);
		if(hr!=S_OK && pFormat->cbSize==0)
		{
			WAVEFORMATEXTENSIBLE *pExFormat = toWaveExtensible(pFormat);
			hr = m_pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE,reinterpret_cast<const WAVEFORMATEX *>(pExFormat),0);
			delete pExFormat;
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

		if(hr!=S_OK && pFormat->cbSize==0)
		{
			WAVEFORMATEXTENSIBLE *pExFormat = toWaveExtensible(pFormat);
			if(pExFormat!=0)
			{
				hr = m_pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,reinterpret_cast<const WAVEFORMATEX *>(pExFormat),&pCloseFormat);
			}
			if(pCloseFormat!=0)
			{
				CoTaskMemFree(pCloseFormat);
				pCloseFormat = 0;
			}
			delete pExFormat;
		}
	}
	return (hr==S_OK);
}

//-------------------------------------------------------------------------------------------
// 0 = 16  = 1
// 1 = 20  = 23
// 2 = 24  = 2
// 3 = 28  = 30
// 4 = 32  = 3
// 5 = 32F = 4
//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayer::bitIndexForFDIndex(int nativeBitIdx) const
{
	const int idxs[NUMBER_WASAPI_MAXBITS] = { 1, 23, 2, 30, 3, 4 };
	
	if(nativeBitIdx >= NUMBER_WASAPI_MAXBITS)
	{
		nativeBitIdx = NUMBER_WASAPI_MAXBITS - 1;
	}
	return idxs[nativeBitIdx];
}

//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayer::nativeBitIndexFromFDIndex(int fdBitIndex) const
{
	int nativeBitIndex;
	
	switch(fdBitIndex)
	{
		case 1:  nativeBitIndex = 0; break;
		case 23: nativeBitIndex = 1; break;
		case 2:  nativeBitIndex = 2; break;
		case 30: nativeBitIndex = 3; break;
		case 3:  nativeBitIndex = 4; break;
		default:
			nativeBitIndex = 5;
			break;
	}
	return nativeBitIndex;
}

//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayer::getIndexOfBits(const WAVEFORMATEX *pFormat) const
{
	int index;
	
	if(pFormat->wFormatTag==e_formatFloat)
	{
		index = 4;
	}
	else
	{
		index = (pFormat->wBitsPerSample >> 3) - 1;
	}
	return index;
}

//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayer::getIndexOfChannels(const WAVEFORMATEX *pFormat) const
{
	return pFormat->nChannels - 1;
}

//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayer::getIndexOfFrequency(const WAVEFORMATEX *pFormat) const
{
	int index;
	
	switch(pFormat->nSamplesPerSec)
	{
		case 8000:
			index = 0;
			break;
		case 11025:
			index = 1;
			break;
		case 12000:
			index = 2;
			break;
		case 16000:
			index = 3;
			break;
		case 22050:
			index = 4;
			break;
		case 24000:
			index = 5;
			break;
		case 32000:
			index = 6;
			break;
		case 44100:
			index = 7;
			break;
		case 48000:
			index = 8;
			break;
		case 64000:
			index = 9;
			break;
		case 88200:
			index = 10;
			break;
		case 96000:
			index = 11;
			break;
		case 176400:
			index = 12;
			break;
		case 192000:
			index = 13;
			break;
		case 352800:
			index = 14;
			break;
		case 384000:
			index = 15;
			break;
		case 705600:
			index = 16;
			break;
		case 768000:
			index = 17;
			break;
		default:
			index = -1;
			break;
	}
	return index;
}

//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayer::getNumberOfBitsFromIndex(int idx) const
{
	int noBits;
	
	if(idx>=0 && idx<5)
	{
		noBits = (idx << 2) + 16;
	}
	else
	{
		noBits = -1;
	}
	return noBits;
}

//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayer::getNumberOfChannelsFromIndex(int idx) const
{
	return idx + 1;
}

//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayer::getFrequencyFromIndex(int idx) const
{
	int freq;
	
	switch(idx)
	{
		case 0:
			freq = 8000;
			break;
		case 1:
			freq = 11025;
			break;
		case 2:
			freq = 12000;
			break;
		case 3:
			freq = 16000;
			break;
		case 4:
			freq = 22050;
			break;
		case 5:
			freq = 24000;
			break;
		case 6:
			freq = 32000;
			break;
		case 8:
			freq = 48000;
			break;
		case 9:
			freq = 64000;
			break;
		case 10:
			freq = 88200;
			break;
		case 11:
			freq = 96000;
			break;
		case 12:
			freq = 176400;
			break;
		case 13:
			freq = 192000;
			break;
		case 14:
			freq = 352800;
			break;
		case 15:
			freq = 384000;
			break;
		case 16:
			freq = 705600;
			break;
		case 17:
			freq = 768000;
			break;
			
		case 7:
		default:
			freq = 44100;
			break;
	}
	return freq;
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::isFormat(int chIdx,int bitIdx,int freqIdx) const
{
	bool res;
	
	if(isExclusive())
	{
		res = (m_formatsExclusive[chIdx][bitIdx][freqIdx] > 0) ? true : false;
	}
	else
	{
		res = (m_formatsShared[chIdx][bitIdx][freqIdx] > 0) ? true : false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

tint WasAPIDeviceLayer::getNoOfBitsFromWaveFormat(const WAVEFORMATEX *pFormat) const
{
	return pFormat->wBitsPerSample;
}

//-------------------------------------------------------------------------------------------

tint WasAPIDeviceLayer::getNoOfChannelsFromWaveFormat(const WAVEFORMATEX *pFormat) const
{
	return pFormat->nChannels;
}

//-------------------------------------------------------------------------------------------

tint WasAPIDeviceLayer::getFrequencyFromWaveFormat(const WAVEFORMATEX *pFormat) const
{
	return pFormat->nSamplesPerSec;
}

//-------------------------------------------------------------------------------------------

QString WasAPIDeviceLayer::id() const
{
	LPWSTR pName = 0;
	QString id;
	
	if(m_pDevice->GetId(&pName)==S_OK && pName!=0)
	{
		id = QString::fromUtf16(reinterpret_cast<const char16_t *>(pName));
	}
	if(pName!=0)
	{
		CoTaskMemFree(pName);
	}
	return id;
}

//-------------------------------------------------------------------------------------------

QString WasAPIDeviceLayer::name() const
{
	HRESULT hr;
	QString deviceName;
	IPropertyStore *iProperties = 0;
	
	hr = m_pDevice->OpenPropertyStore(STGM_READ,&iProperties);
	if(hr==S_OK && iProperties!=0)
	{
		PROPVARIANT varName;
		IPropertyStoreIFSPtr pPropertyStore = createPropertyStoreIF(iProperties);
		
		PropVariantInit(&varName);
		
		hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName,&varName);
		if(hr==S_OK)
		{
			deviceName = QString::fromUtf16(reinterpret_cast<const char16_t *>(varName.pwszVal));
		}
		PropVariantClear(&varName);
	}
	return deviceName;
}

//-------------------------------------------------------------------------------------------

QString WasAPIDeviceLayer::settingsKey(bool exclusive) const
{
	QString key = "Formats_" + id() + "_";
	key += (exclusive) ? "E" : "M";
	return key;
}

//-------------------------------------------------------------------------------------------

QString WasAPIDeviceLayer::settingsValidKey(bool exclusive) const
{
	QString validKey = settingsKey(exclusive) + "_valid";
	return validKey;
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::loadFormats()
{
	return (loadFormats(true) && loadFormats(false));
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::loadFormats(bool exclusive)
{
	QString validKey;
	QString key = settingsKey(exclusive);
	QSettings settings;
	bool res = false;
	
	if(!key.isEmpty())
	{
		QByteArray mem;
	
		settings.beginGroup("wasapi");
		if(settings.contains(key))
		{
			validKey = settingsValidKey(exclusive);
			if(settings.contains(validKey) && settings.value(validKey).toBool())
			{
				mem = settings.value(key).toByteArray();
				if(mem.size() == (NUMBER_WASAPI_MAXCHANNELS * NUMBER_WASAPI_MAXBITS * NUMBER_WASAPI_MAXFREQUENCIES))
				{
					res = true;
				}
			}
		}
		settings.endGroup();
		
		if(res)
		{
			int i,j,k,idx;
			const tbyte *fmts = mem.constData();
			
			for(i=0,idx=0;i<NUMBER_WASAPI_MAXCHANNELS;i++)
			{
				for(j=0;j<NUMBER_WASAPI_MAXBITS;j++)
				{
					for(k=0;k<NUMBER_WASAPI_MAXFREQUENCIES;k++,idx++)
					{
						if(exclusive)
						{
							m_formatsExclusive[i][j][k] = static_cast<tint>(fmts[idx]);
						}
						else
						{
							m_formatsShared[i][j][k] = static_cast<tint>(fmts[idx]);
						}
					}
				}
			}
		}
		else
		{
			blank(exclusive);
		}
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::saveFormats()
{
	return (saveFormats(true) && saveFormats(false));
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::saveFormats(bool exclusive)
{
	tint i,j,k,idx;
	tbyte fmts[NUMBER_WASAPI_MAXCHANNELS * NUMBER_WASAPI_MAXBITS * NUMBER_WASAPI_MAXFREQUENCIES];
	bool res = false;
	
	for(i=0,idx=0;i<NUMBER_WASAPI_MAXCHANNELS;i++)
	{
		for(j=0;j<NUMBER_WASAPI_MAXBITS;j++)
		{
			for(k=0;k<NUMBER_WASAPI_MAXFREQUENCIES;k++,idx++)
			{
				if(exclusive)
				{
					fmts[idx] = static_cast<tbyte>(m_formatsExclusive[i][j][k]);
				}
				else
				{
					fmts[idx] = static_cast<tbyte>(m_formatsShared[i][j][k]);
				}
			}
		}
	}
	
	QString key = settingsKey(exclusive);
	QString validKey = settingsValidKey(exclusive);
	
	if(!key.isEmpty())
	{
		QByteArray mem(fmts,NUMBER_WASAPI_MAXCHANNELS * NUMBER_WASAPI_MAXBITS * NUMBER_WASAPI_MAXFREQUENCIES);
		QSettings settings;
		
		settings.beginGroup("wasapi");
		key = settingsKey(exclusive);
		settings.setValue(key,QVariant(mem));
		// settings.setValue(validKey, QVariant(true));
		settings.setValue(validKey, QVariant(false));
		settings.endGroup();
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::setFrequencyInWaveFormat(int frequency,WAVEFORMATEX *pFormat) const
{
	pFormat->nSamplesPerSec = frequency;
	pFormat->nAvgBytesPerSec = frequency * pFormat->nBlockAlign;
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::setChannelsInWaveFormat(int channel,WAVEFORMATEX *pFormat) const
{
	pFormat->nChannels = channel;
	pFormat->nBlockAlign = (pFormat->wBitsPerSample >> 3) * channel;
	pFormat->nAvgBytesPerSec = pFormat->nSamplesPerSec * pFormat->nBlockAlign;
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::isExclusiveFromAM(AccessModeSharedDevice accessMode) const
{
	if(accessMode == e_Settings)
	{
		return isExclusive();
	}
	else
	{
		return (accessMode == e_Exclusive);
	}
}

//-------------------------------------------------------------------------------------------

QSet<int> WasAPIDeviceLayer::queryFrequencyCapabilities(AccessModeSharedDevice accessMode)
{
	bool exclusive;
	int i,j,k;
	QSet<int> frequencySet;
	
	exclusive = isExclusiveFromAM(accessMode);

	for(i=0;i<NUMBER_WASAPI_MAXCHANNELS;i++)
	{
		for(j=0;j<NUMBER_WASAPI_MAXBITS;j++)
		{
			for(k=0;k<NUMBER_WASAPI_MAXFREQUENCIES;k++)
			{
				if((exclusive && m_formatsExclusive[i][j][k] > 0) || (!exclusive && m_formatsShared[i][j][k] > 0))
				{
					int freq = getFrequencyFromIndex(k);
					QSet<int>::iterator ppI = frequencySet.find(freq);
					if(ppI==frequencySet.end())
					{
						frequencySet.insert(freq);
					}
				}
			}
		}
	}
	return frequencySet;
}

//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayer::queryChannelCapabilities(AccessModeSharedDevice accessMode)
{
	bool exclusive = isExclusiveFromAM(accessMode);
	int i,j,k,maxChs = 0;
	
	for(i=0;i<NUMBER_WASAPI_MAXCHANNELS;i++)
	{
		int noChs = getNumberOfChannelsFromIndex(i);

		for(j=0;j<NUMBER_WASAPI_MAXBITS;j++)
		{
			for(k=0;k<NUMBER_WASAPI_MAXFREQUENCIES;k++)
			{
				if(exclusive)
				{
					if(m_formatsExclusive[i][j][k] > 0 && noChs > maxChs)
					{
						maxChs = noChs;
					}
				}
				else
				{
					if(m_formatsShared[i][j][k] > 0 && noChs > maxChs)
					{
						maxChs = noChs;
					}				
				}
			}
		}
	}
	return maxChs;
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::queryDeviceFormatCapabilities()
{
	bool exclusiveFlag;
	int i,j,k,l;
	
	for(l=0;l<2;l++)
	{
		exclusiveFlag = (!l) ? true : false;
		for(i=0;i<NUMBER_WASAPI_MAXCHANNELS;i++)
		{
			for(j=0;j<NUMBER_WASAPI_MAXBITS;j++)
			{
				for(k=0;k<NUMBER_WASAPI_MAXFREQUENCIES;k++)
				{
					hasIndexedFormatWithUpdate(j, i, k, exclusiveFlag);
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::hasIndexedFormatWithUpdate(tint bitIdx, tint chIdx, tint freqIdx, bool exclusive)
{
	bool res = false;
	
	if(exclusive)
	{
		if(m_formatsExclusive[chIdx][bitIdx][freqIdx] < 0)
		{
			m_formatsExclusive[chIdx][bitIdx][freqIdx] = queryFormatIndexCapability(bitIdx, chIdx, freqIdx, true);
		}
		if(m_formatsExclusive[chIdx][bitIdx][freqIdx] > 0)
		{
			res = true;
		}
	}
	else
	{
		if(m_formatsShared[chIdx][bitIdx][freqIdx] < 0)
		{
			m_formatsShared[chIdx][bitIdx][freqIdx] = queryFormatIndexCapability(bitIdx, chIdx, freqIdx, false);
		}
		if(m_formatsShared[chIdx][bitIdx][freqIdx] > 0)
		{
			res = true;
		}	
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::setWaveFormatFromIndex(tint bitIdx, tint chIdx, tint freqIdx, WAVEFORMATEX& format)
{
	int noChannels = getNumberOfChannelsFromIndex(chIdx);
	int noBits = getNumberOfBitsFromIndex(bitIdx);
	int frequency = getFrequencyFromIndex(freqIdx);
	
	defaultWaveFormat(format);
	if(noBits > 0)
	{
		setWaveFormat(noChannels,noBits,frequency,format);
	}
	else
	{
		format.wFormatTag = e_formatFloat;
		setWaveFormat(noChannels,32,frequency,format);
	}
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::setWaveExtensibleFormatFromIndex(tint bitIdx, tint chIdx, tint freqIdx, WAVEFORMATEXTENSIBLE& format)
{
	setWaveExtensibleFormatFromIndex(bitIdx, chIdx, freqIdx, false, format);
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::setWaveExtensibleFormatFromIndex(tint bitIdx, tint chIdx, tint freqIdx, bool is32Bit, WAVEFORMATEXTENSIBLE& format)
{
	int noChannels = getNumberOfChannelsFromIndex(chIdx);
	int noBits = getNumberOfBitsFromIndex(bitIdx);
	int frequency = getFrequencyFromIndex(freqIdx);
	
	defaultWaveExtensibleFormat(format);
	if(noBits > 0)
	{
		if(is32Bit)
		{
			setWaveExtensibleFormat(noChannels, noBits, 4, frequency, format);
		}
		else
		{
			setWaveExtensibleFormat(noChannels, noBits, frequency, format);
		}		
	}
	else
	{
		setWaveExtensibleFloatFormatFromIndex(chIdx, freqIdx, false, format);
	}
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::setWaveExtensibleFloatFormatFromIndex(tint chIdx, tint freqIdx, bool is64Bit, WAVEFORMATEXTENSIBLE& format)
{
	int noChannels = getNumberOfChannelsFromIndex(chIdx);
	int frequency = getFrequencyFromIndex(freqIdx);
	
	defaultWaveExtensibleFloatFormat(format, is64Bit);
	setWaveExtensibleFloatFormat(noChannels, frequency, is64Bit, format);
}

//-------------------------------------------------------------------------------------------

FormatDescription WasAPIDeviceLayer::descriptionFromWaveFormat(const WAVEFORMATEX *pFormat)
{
	FormatDescription desc;

	desc.setBitsIndex(nativeBitIndexFromFDIndex(getIndexOfBits(pFormat)));
	desc.setChannelsIndex(getIndexOfChannels(pFormat));
	desc.setFrequencyIndex(getIndexOfFrequency(pFormat));
	return desc;
}

//-------------------------------------------------------------------------------------------

WAVEFORMATEX *WasAPIDeviceLayer::descriptionToWaveFormat(const FormatDescription& desc)
{
	WAVEFORMATEX *pFormat = new WAVEFORMATEX;
	setWaveFormatFromIndex(nativeBitIndexFromFDIndex(desc.bitsIndex()),desc.channelsIndex(),desc.frequencyIndex(),*pFormat);
	return pFormat;
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::populateFormatsSupported(FormatsSupported& support)
{
	bool exclusive = isExclusive();

	support.clear();

	for(tint i=0;i<NUMBER_WASAPI_MAXCHANNELS;i++)
	{
		for(tint j=0;j<NUMBER_WASAPI_MAXBITS;j++)
		{
			for(tint k=0;k<NUMBER_WASAPI_MAXFREQUENCIES;k++)
			{
				if(hasIndexedFormatWithUpdate(j, i, k, exclusive))
				{
					FormatDescription desc;
					
					desc.setBitsIndex(bitIndexForFDIndex(j));
					desc.setChannelsIndex(i);
					desc.setFrequencyIndex(k);
					
					support.add(desc);
				}
			}		
		}
	}
}

//-------------------------------------------------------------------------------------------

WAVEFORMATEX *WasAPIDeviceLayer::supportedWaveFormatFromDescription(const FormatDescription& desc)
{
	tint bitIdx, chIdx, freqIdx, entry, type;

	bitIdx = nativeBitIndexFromFDIndex(desc.bitsIndex());
	chIdx = desc.channelsIndex();
	freqIdx = desc.frequencyIndex();
	entry = (isExclusive()) ? m_formatsExclusive[chIdx][bitIdx][freqIdx] : m_formatsShared[chIdx][bitIdx][freqIdx];
	
	if(desc.bits() == 16)
	{
		type = (entry & WAVE_EXT_16) ? WAVE_EXT_16 : WAVE_BASIC_16;
	}
	else if(desc.bits() == 20)
	{
		type = (entry & WAVE_EXT_24) ? WAVE_EXT_24 : WAVE_EXT_32;
	}
	else if(desc.bits() == 24)
	{
		if(entry & WAVE_EXT_24)
		{
			type = WAVE_EXT_24;
		}
		else
		{
			type = (entry & WAVE_EXT_32) ? WAVE_EXT_32 : WAVE_BASIC_24;
		}
	}
	else if(desc.bits() == 28)
	{
		type = WAVE_EXT_32;
	}
	else if(desc.bits() == 32 && desc.typeOfData() == FormatDescription::e_DataSignedInteger)
	{
		type = (entry & WAVE_EXT_32) ? WAVE_EXT_32 : WAVE_BASIC_32;
	}
	else
	{
		type = WAVE_FLOAT_32;
	}
	return waveFormatFromType(desc.channels(), desc.bits(), desc.frequency(), type);
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::findClosestDescription(const FormatDescription& sourceDesc, FormatDescription& descClosest)
{
	FormatsSupported support;
	
	populateFormatsSupported(support);
	return FormatDescriptionUtils::findClosestFormatType(sourceDesc, support, descClosest);
}

//-------------------------------------------------------------------------------------------

WAVEFORMATEX *WasAPIDeviceLayer::findClosestSupportedFormat(const FormatDescription& sourceDesc)
{
	WAVEFORMATEX *pFormat = NULL;
	FormatDescription closestDesc;

	if(m_pAudioClient.isNull())
	{
		return 0;
	}
	if(findClosestDescription(sourceDesc, closestDesc))
	{
		pFormat = supportedWaveFormatFromDescription(closestDesc);
		if(pFormat != NULL)
		{
			HRESULT hr;
			WAVEFORMATEX* pCloseFormat = 0;
			
			if(isExclusive())
			{
				hr = getAudioClient()->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, pFormat, 0);
			}
			else
			{
				hr = getAudioClient()->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, pFormat, &pCloseFormat);
				if(pCloseFormat!=0)
				{
					CoTaskMemFree(pCloseFormat);
				}
			}
			if(hr != S_OK)
			{
				delete pFormat;
				pFormat = NULL;
			}
		}
	}
	return pFormat;
}

//-------------------------------------------------------------------------------------------

IAudioClientIFSPtr WasAPIDeviceLayer::getAudioClient()
{
	if(m_pAudioClient.isNull() && !m_pDevice.isNull())
	{
		HRESULT hr;
		IAudioClient *pAudioClient = 0;
		const IID IDOF_IAudioClient = __uuidof(IAudioClient);
		
		hr = m_pDevice->Activate(IDOF_IAudioClient,CLSCTX_ALL,0,reinterpret_cast<void **>(&pAudioClient));
		if(hr==S_OK && pAudioClient!=0)
		{
			m_pAudioClient = createAudioClientIF(pAudioClient);
		}
	}
	return m_pAudioClient;
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::releaseAudioClient()
{
	releaseVolumeIF();
	if(!m_pAudioClient.isNull())
	{
		m_pAudioClient.clear();
	}
}

//-------------------------------------------------------------------------------------------

ISimpleAudioVolumeIFSPtr WasAPIDeviceLayer::getVolumeSharedIF()
{
	if(m_pVolumeShared.isNull() && !m_pAudioClient.isNull())
	{
		HRESULT hr;
		ISimpleAudioVolume *pVolume = 0;
		const IID IDOF_ISimpleAudioVolume = __uuidof(ISimpleAudioVolume);
		
		hr = m_pAudioClient->GetService(IDOF_ISimpleAudioVolume, reinterpret_cast<void **>(&pVolume));
		if(hr == S_OK && pVolume != 0)
		{
			ISimpleAudioVolumeIFSPtr pVolumeIF(new ISimpleAudioVolumeIF(pVolume));
			m_pVolumeShared = pVolumeIF;
		}
	}
	return m_pVolumeShared;
}

//-------------------------------------------------------------------------------------------

IAudioEndpointVolumeIFSPtr WasAPIDeviceLayer::getVolumeExclusiveIF()
{
	if(m_pVolumeExclusive.isNull() && !m_pDevice.isNull())
	{
		HRESULT hr;
		IAudioEndpointVolume *pVolume = 0;
		const IID IDOF_IAudioEndpointVolume = __uuidof(IAudioEndpointVolume);
		
		hr = m_pDevice->Activate(IDOF_IAudioEndpointVolume, CLSCTX_ALL, 0, reinterpret_cast<void **>(&pVolume));
		if(hr == S_OK && pVolume != 0)
		{
			IAudioEndpointVolumeIFSPtr pVolumeIF(new IAudioEndpointVolumeIF(pVolume));
			m_pVolumeExclusive = pVolumeIF;
		}
	}
	return m_pVolumeExclusive;
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::releaseVolumeIF()
{
	if(!m_pVolumeExclusive.isNull())
	{
		m_pVolumeExclusive.clear();
	}
	if(!m_pVolumeShared.isNull())
	{
		m_pVolumeShared.clear();
	}
}

//-------------------------------------------------------------------------------------------

QString WasAPIDeviceLayer::waveFormatTypeName(WORD wFormatTag)
{
	QString name;
	
	switch(wFormatTag)
	{
		case WAVE_FORMAT_PCM: name = "PCM"; break;
		case WAVE_FORMAT_IEEE_FLOAT: name = "PCM-Float"; break;
		case WAVE_FORMAT_DRM: name = "DRM"; break;
		case WAVE_FORMAT_EXTENSIBLE: name = "Extensible"; break;
		case WAVE_FORMAT_ALAW: name = "A-Law"; break;
		case WAVE_FORMAT_MULAW: name = "Mu-Law"; break;
		case WAVE_FORMAT_ADPCM: name = "AD-PCM"; break;
		case WAVE_FORMAT_MPEG: name = "MPEG-1"; break;
		case WAVE_FORMAT_DOLBY_AC3_SPDIF: name = "DD-AC3"; break;
		case WAVE_FORMAT_WMASPDIF: name = "WMA"; break;
		default:
			name = QString("Unknown (0x") + QString::number(static_cast<uint>(wFormatTag), 16) + QString(")");
			break;
	}
	return name;
}

//-------------------------------------------------------------------------------------------

QString WasAPIDeviceLayer::waveFormatGUIDType(GUID guid)
{
	QString type;
	
	if(IsEqualGUID(guid, KSDATAFORMAT_SUBTYPE_PCM))
	{
		type = "PCM";
	}
	else if(IsEqualGUID(guid, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
	{
		type = "PCM-Float";
	}
	else if(IsEqualGUID(guid, KSDATAFORMAT_SUBTYPE_DRM))
	{
		type = "DRM";
	}
	else if(IsEqualGUID(guid, KSDATAFORMAT_SUBTYPE_ALAW))
	{
		type = "A-Law";
	}
	else if(IsEqualGUID(guid, KSDATAFORMAT_SUBTYPE_MULAW))
	{
		type = "Mu-Law";
	}
	else if(IsEqualGUID(guid, KSDATAFORMAT_SUBTYPE_ADPCM))
	{
		type = "AD-PCM";
	}
	else
	{
		type = "????";
	}
	return type;
}

//-------------------------------------------------------------------------------------------

QString WasAPIDeviceLayer::guidString(GUID id)
{
	tuint16 dataTG;
	QString g;
	common::BString x;
	
	x = common::BString::HexInt(static_cast<tuint32>(id.Data1), 8, true);
	g += static_cast<const tchar *>(x);
	g += "-";
	
	x = common::BString::HexInt(static_cast<tuint32>(id.Data2), 4, true);
	g += static_cast<const tchar *>(x);
	g += "-";
	
	x = common::BString::HexInt(static_cast<tuint32>(id.Data3), 4, true);
	g += static_cast<const tchar *>(x);
	g += "-";
	
	dataTG = engine::to16BitUnsignedFromLittleEndian(reinterpret_cast<const tchar *>(id.Data4));
	x = common::BString::HexInt(static_cast<tuint32>(dataTG), 4, true);
	g += static_cast<const tchar *>(x);
	g += "-";
	
	for(int i = 2; i < 8; i++)
	{
		x = common::BString::HexInt(static_cast<tuint32>(id.Data4[i]), 2, true);
		g += static_cast<const tchar *>(x);
	}
	return g;
}

//-------------------------------------------------------------------------------------------

QString WasAPIDeviceLayer::printChannelMask(DWORD dwChannelMask)
{
	static const char *speakerNames[] = {
		// 0 - #define SPEAKER_FRONT_LEFT              0x1
		"Front Left",
		// 1 - #define SPEAKER_FRONT_RIGHT             0x2
		"Front Right",
		// 2 - #define SPEAKER_FRONT_CENTER            0x4
		"Front Center",
		// 3 - #define SPEAKER_LOW_FREQUENCY           0x8
		"LFE (Subwoofer)",
		// 4 - #define SPEAKER_BACK_LEFT               0x10
		"Back Left",
		// 5 - #define SPEAKER_BACK_RIGHT              0x20
		"Back Right",
		// 6 - #define SPEAKER_FRONT_LEFT_OF_CENTER    0x40
		"Front Left of Center",
		// 7 - #define SPEAKER_FRONT_RIGHT_OF_CENTER   0x80
		"Front Right of Center",
		// 8 - #define SPEAKER_BACK_CENTER             0x100
		"Back Center",
		// 9 - #define SPEAKER_SIDE_LEFT               0x200
		"Side Left",
		// 10 - #define SPEAKER_SIDE_RIGHT              0x400
		"Side Right",
		// 11 - #define SPEAKER_TOP_CENTER              0x800
		"Top Center",
		// 12 - #define SPEAKER_TOP_FRONT_LEFT          0x1000
		"Top Front Left",
		// 13 - #define SPEAKER_TOP_FRONT_CENTER        0x2000
		"Top Front Center",
		// 14 - #define SPEAKER_TOP_FRONT_RIGHT         0x4000
		"Top Front Right",
		// 15 - #define SPEAKER_TOP_BACK_LEFT           0x8000
		"Top Back Left",
		// 16 - #define SPEAKER_TOP_BACK_CENTER         0x10000
		"Top Back Center",
		// 17 - #define SPEAKER_TOP_BACK_RIGHT          0x20000
		"Top Back Right"
	};
	
	QString speakers;
	
	for(int i = 0; i < 18; i++)
	{
		DWORD mask = 1 << i;
		
		if(mask & dwChannelMask)
		{
			if(i)
			{
				speakers += ", ";
			}
			speakers += speakerNames[i];
		}
	}
	return speakers;
}

//-------------------------------------------------------------------------------------------
/*
PCM ch=2, freq=44100, bits=16, ba=4
Extensible ch=2, freq=44100, bits=16, ba=4, vbps=16
	Speakers: Front Left, Front Right
	GUID: 00000000-0000-0000-0000-000000000000 (KSDATAFORMAT_SUBTYPE_PCM)
*/
//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::printWaveFormat(WAVEFORMATEX *pFormat)
{
	QString formatString;
	formatString = waveFormatTypeName(pFormat->wFormatTag);

	common::Log::g_Log << waveFormatTypeName(pFormat->wFormatTag) << " ";
	common::Log::g_Log << "ch=" << QString::number(pFormat->nChannels) << ", ";
	common::Log::g_Log << "freq=" << QString::number(pFormat->nSamplesPerSec) << ", ";
	common::Log::g_Log << "ba=" << QString::number(pFormat->nBlockAlign);
	if(pFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		WAVEFORMATEXTENSIBLE *pFormatExt = reinterpret_cast<WAVEFORMATEXTENSIBLE *>(pFormat);
	
		common::Log::g_Log << ", vbps=" << QString::number(pFormatExt->Samples.wValidBitsPerSample) << common::c_endl;
		common::Log::g_Log << "\tSpeakers: " << printChannelMask(pFormatExt->dwChannelMask) << common::c_endl;
		common::Log::g_Log << "\tGUID: " << guidString(pFormatExt->SubFormat) << " (" << waveFormatGUIDType(pFormatExt->SubFormat) << ")" << common::c_endl;
	
		if(pFormat->cbSize < (sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX)))
		{
			common::Log::g_Log << "\tExpected WAVE_FORMAT_EXTENSIBLE too small" << common::c_endl;
		}
	}
	else
	{
		common::Log::g_Log << common::c_endl;
	}
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::printIsFormatSupported(WAVEFORMATEX *pFormat, bool isExcl)
{
	HRESULT hr;
	WAVEFORMATEX *pCloseFormat = 0;

	if(isExcl)
		hr = getAudioClient()->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, pFormat, 0);
	else
		hr = getAudioClient()->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, pFormat, &pCloseFormat);

	if(hr == S_OK)
		common::Log::g_Log << "S_OK Succeeded and the audio endpoint device supports the specified stream format.";
	else if(hr == S_FALSE)
		common::Log::g_Log << "S_FALSE Succeeded with a closest match to the specified format.";
	else if(hr == AUDCLNT_E_UNSUPPORTED_FORMAT)
		common::Log::g_Log << "AUDCLNT_E_UNSUPPORTED_FORMAT Succeeded but the specified format is not supported in exclusive mode.";
	else if(hr == E_POINTER)
		common::Log::g_Log << "E_POINTER Parameter pFormat is NULL, or ppClosestMatch is NULL and ShareMode is AUDCLNT_SHAREMODE_SHARED.";
	else if(hr == E_INVALIDARG)
		common::Log::g_Log << "E_INVALIDARG Parameter ShareMode is a value other than AUDCLNT_SHAREMODE_SHARED or AUDCLNT_SHAREMODE_EXCLUSIVE.";
	else if(hr == AUDCLNT_E_DEVICE_INVALIDATED)
		common::Log::g_Log << "AUDCLNT_E_DEVICE_INVALIDATED The audio endpoint device has been unplugged, or the audio hardware or associated hardware resources have been reconfigured, disabled, removed, or otherwise made unavailable for use.";
	else if(hr == AUDCLNT_E_SERVICE_NOT_RUNNING)
		common::Log::g_Log << "AUDCLNT_E_SERVICE_NOT_RUNNING The Windows audio service is not running.";
	else
		common::Log::g_Log << "Unknown error code HRESULT=" << QString::number(hr, 16);
	common::Log::g_Log << common::c_endl;
	
	if(pCloseFormat != 0)
	{
		printWaveFormat(pCloseFormat);
		CoTaskMemFree(pCloseFormat);
	}	
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::printIndexedFormatSupport(tint bitIdx,tint chIdx,tint freqIdx)
{
	WAVEFORMATEX format;
	WAVEFORMATEXTENSIBLE formatPCMEx, formatFloatEx;

	common::Log::g_Log << "Format bits=" << QString::number(getNumberOfBitsFromIndex(bitIdx));
	common::Log::g_Log << ", channels=" << QString::number(getNumberOfChannelsFromIndex(chIdx));
	common::Log::g_Log << ", frequency=" << QString::number(getFrequencyFromIndex(freqIdx));
	common::Log::g_Log << common::c_endl;
	
	setWaveFormatFromIndex(bitIdx, chIdx, freqIdx, format);
	setWaveExtensibleFormatFromIndex(bitIdx, chIdx, freqIdx, formatPCMEx);
	setWaveExtensibleFloatFormatFromIndex(chIdx, freqIdx, false, formatFloatEx);
	
	common::Log::g_Log << "**-- Basic WAVE Format" << common::c_endl;
	printWaveFormat(&format);
	common::Log::g_Log << "**-- Extensible WAVE Format" << common::c_endl;
	printWaveFormat(reinterpret_cast<WAVEFORMATEX *>(&formatPCMEx));
	common::Log::g_Log << "**-- Ext-Float WAVE Format" << common::c_endl;
	printWaveFormat(reinterpret_cast<WAVEFORMATEX*>(&formatFloatEx));
	
	common::Log::g_Log << "---- Shared support = " << ((m_formatsShared[chIdx][bitIdx][freqIdx] > 0) ? "TRUE" : "FALSE") << common::c_endl;
	common::Log::g_Log << "-- Basic WAVE Format" << common::c_endl;
	printIsFormatSupported(&format, false);
	common::Log::g_Log << "-- Extensible WAVE Format" << common::c_endl;
	printIsFormatSupported(reinterpret_cast<WAVEFORMATEX*>(&formatPCMEx), false);
	common::Log::g_Log << "-- Ext-Float WAVE Format" << common::c_endl;
	printIsFormatSupported(reinterpret_cast<WAVEFORMATEX*>(&formatFloatEx), false);

	common::Log::g_Log << "**** Exclusive support = " << ((m_formatsExclusive[chIdx][bitIdx][freqIdx] > 0) ? "TRUE" : "FALSE") << common::c_endl;
	common::Log::g_Log << "** Basic WAVE Format" << common::c_endl;
	printIsFormatSupported(&format, true);
	common::Log::g_Log << "** Extensible WAVE Format" << common::c_endl;
	printIsFormatSupported(reinterpret_cast<WAVEFORMATEX*>(&formatPCMEx), true);
	common::Log::g_Log << "** Ext-Float WAVE Format" << common::c_endl;
	printIsFormatSupported(reinterpret_cast<WAVEFORMATEX*>(&formatFloatEx), true);
}

//-------------------------------------------------------------------------------------------
// ( 0) 16 - WAVE_EXT_16, WAVE_BASIC_16
// ( 1) 20 - WAVE_EXT_24, WAVE_EXT_32
// ( 2) 24 - WAVE_EXT_24, WAVE_BASIC_24, WAVE_EXT_32
// ( 3) 28 - WAVE_EXT_32
// ( 4) 32 - WAVE_EXT_32, WAVE_BASIC_32
// (-1) 32F- WAVE_FLOAT_32
//-------------------------------------------------------------------------------------------

WAVEFORMATEX *WasAPIDeviceLayer::waveFormatFromType(tint noChannels, tint noBits, tint frequency, tint type) const
{
	WAVEFORMATEX *format = NULL;
	WAVEFORMATEXTENSIBLE *formatEx;

	if(type & (WAVE_BASIC_16 | WAVE_BASIC_24 | WAVE_BASIC_32))
	{
		format = new WAVEFORMATEX;
		if(format != NULL)
		{
			defaultWaveFormat(*format);
			setWaveFormat(noChannels, noBits, frequency, *format);
		}
	}
	else if(type & (WAVE_EXT_16 | WAVE_EXT_24 | WAVE_EXT_32))
	{
		formatEx = new WAVEFORMATEXTENSIBLE;
		if(formatEx != NULL)
		{
			tint noBytes;
			
			if(type & WAVE_EXT_16)
			{
				noBytes = 2;
			}
			else if(type & WAVE_EXT_24)
			{
				noBytes = 3;
			}
			else
			{
				noBytes = 4;
			}
			defaultWaveExtensibleFormat(*formatEx);
			setWaveExtensibleFormat(noChannels, noBits, noBytes, frequency, *formatEx);
			format = reinterpret_cast<WAVEFORMATEX *>(formatEx);
		}
	}
	else if(type & WAVE_FLOAT_32)
	{
		formatEx = new WAVEFORMATEXTENSIBLE;
		if(formatEx != NULL)
		{
			defaultWaveExtensibleFloatFormat(*formatEx, false);
			setWaveExtensibleFloatFormat(noChannels, frequency, false, *formatEx);
			format = reinterpret_cast<WAVEFORMATEX *>(formatEx);
		}	
	}
	return format;
}

//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayer::queryFormatIndexCapability(tint bitIdx, tint chIdx, tint freqIdx, tint isExculsive)
{
	int noBits, noChannels, frequency, caps, type;
	WAVEFORMATEX *format;
	QVector<int> types;

	noBits = getNumberOfBitsFromIndex(bitIdx);
	noChannels = getNumberOfChannelsFromIndex(chIdx);
	frequency = getFrequencyFromIndex(freqIdx);
	
	switch(noBits)
	{
		case 16:
			types.append(WAVE_EXT_16);
			types.append(WAVE_BASIC_16);
			break;
		case 20:
			types.append(WAVE_EXT_24);
			types.append(WAVE_EXT_32);
			break;
		case 24:
			types.append(WAVE_EXT_24);
			types.append(WAVE_BASIC_24);
			types.append(WAVE_EXT_32);
			break;
		case 28:
			types.append(WAVE_EXT_32);
			break;
		case 32:
			types.append(WAVE_EXT_32);
			types.append(WAVE_BASIC_32);
			break;
		default:
			types.append(WAVE_FLOAT_32);
			break;
	}
	
	caps = 0;
	for(QVector<int>::iterator ppI = types.begin(); ppI != types.end(); ppI++)
	{
		type = *ppI;
		format = waveFormatFromType(noChannels, noBits, frequency, type);
		if(format != NULL)
		{
			HRESULT hr;
			WAVEFORMATEX* pCloseFormat = 0;
			
			if (isExculsive)
			{
				hr = getAudioClient()->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, format, 0);
			}
			else
			{
				hr = getAudioClient()->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, format, &pCloseFormat);
			}
			if(hr == S_OK)
			{
				caps |= type;
			}
			if(pCloseFormat!=0)
			{
				CoTaskMemFree(pCloseFormat);
				pCloseFormat = 0;
			}
			delete format;
		}
	}
	return caps;
}

//-------------------------------------------------------------------------------------------

QString WasAPIDeviceLayer::capabilityCSVIndexed(tint bitIdx, tint chIdx, tint freqIdx, tint isExculsive)
{
	QString c;
	int caps;
	
	caps = queryFormatIndexCapability(bitIdx, chIdx, freqIdx, isExculsive);
	if(caps)
	{
		common::BString n = common::BString::HexInt(caps, 2);
		c = "Yes (0x";
		c += static_cast<const tchar *>(n);
		c += ")";
	}
	else
	{
		c = "No";
	}
	return c;
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::isExclusive() const
{
	return AudioSettings::instance(name())->isExclusive();
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::setExclusive(bool flag)
{
	AudioSettings::instance(name())->setExclusive(flag);
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::isDeviceVolume()
{
	bool res;
	
	if(isExclusive())
	{
		res = isDeviceVolumeExclusive();
	}
	else
	{
		res = isDeviceVolumeShared();
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::isDeviceVolumeShared()
{
	bool res = false;
	ISimpleAudioVolumeIFSPtr pVolume = getVolumeSharedIF();
	
	if(!pVolume.isNull())
	{
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::isDeviceVolumeExclusive()
{
	bool res = false;
	IAudioEndpointVolumeIFSPtr pVolume = getVolumeExclusiveIF();
	
	if(!pVolume.isNull())
	{
		DWORD mask = 0;
		
		if(pVolume->QueryHardwareSupport(&mask) == S_OK)
		{
			res = (mask & ENDPOINT_HARDWARE_SUPPORT_VOLUME) ? true : false;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

sample_t WasAPIDeviceLayer::getVolume()
{
	sample_t vol;
	
	if(isExclusive())
	{
		vol = getVolumeExclusive();
	}
	else
	{
		vol = getVolumeShared();
	}
	if(vol < 0.0)
	{
		vol = 0.0;
	}
	else if(vol > 1.0)
	{
		vol = 1.0;
	}
	return vol;
}

//-------------------------------------------------------------------------------------------

sample_t WasAPIDeviceLayer::getVolumeShared()
{
	sample_t vol = 1.0;
	ISimpleAudioVolumeIFSPtr pVolume = getVolumeSharedIF();
	
	if(!pVolume.isNull())
	{
		BOOL isMute = FALSE;
		pVolume->GetMute(&isMute);
		if(!isMute)
		{
			HRESULT hr;
			float pVol = 1.0f;

			hr = pVolume->GetMasterVolume(&pVol);
			if(hr == S_OK)
			{
				vol = static_cast<sample_t>(pVol);
			}
		}
	}
	return vol;
}

//-------------------------------------------------------------------------------------------

sample_t WasAPIDeviceLayer::getVolumeExclusive()
{
	sample_t vol = 1.0;
	IAudioEndpointVolumeIFSPtr pVolume = getVolumeExclusiveIF();
	
	if(!pVolume.isNull())
	{
		BOOL isMute = FALSE;
		pVolume->GetMute(&isMute);
		if(!isMute)
		{
			float pVol = 1.0f;
			if(pVolume->GetMasterVolumeLevelScalar(&pVol) == S_OK)
			{
				vol = static_cast<sample_t>(pVol);
			}
		}		
	}
	return vol;
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::setVolume(sample_t vol)
{
	bool res;

	if(vol < 0.0)
	{
		vol = 0.0;
	}
	else if(vol > 1.0)
	{
		vol = 1.0;
	}

	if(isExclusive())
	{
		res = setVolumeExclusive(vol);
	}
	else
	{
		res = setVolumeShared(vol);
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::setVolumeShared(sample_t vol)
{
	bool res = false;
	ISimpleAudioVolumeIFSPtr pVolume = getVolumeSharedIF();
	const GUID volGUID = GUID_OMEGA_VOLUME_EVENTS;
	
	if(!pVolume.isNull())
	{
		res = true;
		if(isEqual(vol, 0.0))
		{
			BOOL isMute = TRUE;
		
			pVolume->GetMute(&isMute);
			if(!isMute)
			{
				if(pVolume->SetMute(TRUE, &volGUID) != S_OK)
				{
					res = false;
				}
			}
		}
		else
		{
			BOOL isMute = FALSE;
			
			pVolume->GetMute(&isMute);
			if(isMute)
			{
				if(pVolume->SetMute(FALSE, &volGUID) != S_OK)
				{
					res = false;
				}
			}
		}
		
		float pVol = static_cast<float>(vol);
		if(pVolume->SetMasterVolume(pVol, &volGUID) != S_OK)
		{
			res = false;
		}
	}
	return res;	
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::setVolumeExclusive(sample_t vol)
{
	bool res = false;
	IAudioEndpointVolumeIFSPtr pVolume = getVolumeExclusiveIF();
	const GUID volGUID = GUID_OMEGA_VOLUME_EVENTS;
	
	if(!pVolume.isNull())
	{
		res = true;
		if(isEqual(vol, 0.0))
		{
			BOOL isMute = TRUE;
		
			pVolume->GetMute(&isMute);
			if(!isMute)
			{
				if(pVolume->SetMute(TRUE, &volGUID) != S_OK)
				{
					res = false;
				}
			}
		}
		else
		{
			BOOL isMute = FALSE;
			
			pVolume->GetMute(&isMute);
			if(isMute)
			{
				if(pVolume->SetMute(FALSE, &volGUID) != S_OK)
				{
					res = false;
				}
			}
		}
		
		float pVol = static_cast<float>(vol);
		if(pVolume->SetMasterVolumeLevelScalar(pVol, &volGUID) != S_OK)
		{
			res = false;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::setupVolumeNotification(VolumeChangeNotifier pNotifier, LPVOID pVInstance)
{
	bool res;
	
	if(pNotifier != 0)
	{
		shutdownVolumeNotification();
		if(isExclusive())
		{
			res = setupVolumeNotificationExclusive(pNotifier, pVInstance);
		}
		else
		{
			res = setupVolumeNotificationShared(pNotifier, pVInstance);
		}
	}
	else
	{
		res = false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::setupVolumeNotificationShared(VolumeChangeNotifier pNotifier, LPVOID pVInstance)
{
	HRESULT hr;
	IAudioSessionControl *pControl = 0;
	bool res = false;
		
	hr = m_pAudioClient->GetService(__uuidof(IAudioSessionControl), reinterpret_cast<void **>(&pControl));
	if(hr == S_OK && pControl != 0)
	{
		IAudioSessionControlIFSPtr pAudioCtrl(new IAudioSessionControlIF(pControl));
		m_pAudioSessionControl = pAudioCtrl;
		
		WasAPISharedVolumeEvents *pVolEvents = new WasAPISharedVolumeEvents(pNotifier, pVInstance);
		hr = m_pAudioSessionControl->RegisterAudioSessionNotification(reinterpret_cast<IAudioSessionEvents *>(pVolEvents));
		if(hr == S_OK)
		{
			m_pVolumeEventsShared = pVolEvents;
			res = true;
		}
		else
		{
			pVolEvents->Release();
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayer::setupVolumeNotificationExclusive(VolumeChangeNotifier pNotifier, LPVOID pVInstance)
{
	HRESULT hr;
	IAudioEndpointVolumeIFSPtr pVolumeIF = getVolumeExclusiveIF();
	bool res = false;

	if(!pVolumeIF.isNull())
	{
		WasAPIExclusiveVolumeEvents *pVolEvents = new WasAPIExclusiveVolumeEvents(pNotifier, pVInstance);
		hr = pVolumeIF->RegisterControlChangeNotify(reinterpret_cast<IAudioEndpointVolumeCallback *>(pVolEvents));
		if(hr == S_OK)
		{
			m_pVolumeEventsExclusive = pVolEvents;
			res = true;
		}
		else
		{
			pVolEvents->Release();
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayer::shutdownVolumeNotification()
{
	if(m_pVolumeEventsShared != 0)
	{
		if(!m_pAudioSessionControl.isNull())
		{
			m_pAudioSessionControl->UnregisterAudioSessionNotification(reinterpret_cast<IAudioSessionEvents *>(m_pVolumeEventsShared));
		}
		m_pVolumeEventsShared->Release();
		m_pVolumeEventsShared = 0;
	}
	if(m_pVolumeEventsExclusive != 0)
	{
		if(!m_pVolumeExclusive.isNull())
		{
			m_pVolumeExclusive->UnregisterControlChangeNotify(reinterpret_cast<IAudioEndpointVolumeCallback *>(m_pVolumeEventsExclusive));
		}
		m_pVolumeEventsExclusive->Release();
		m_pVolumeEventsExclusive = 0;
	}
	m_pAudioSessionControl.clear();
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
