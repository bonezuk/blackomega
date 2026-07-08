//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOQUERYALSA_H
#define __OMEGA_AUDIOIO_AOQUERYALSA_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOQueryDevice.h"
#include "audioio/inc/LinuxALSAIF.h"
#include "audioio/inc/FormatDescription.h"
#include "audioio/inc/FormatsSupported.h"
#include "audioio/inc/ALSAStreamParser.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryALSA : public AOQueryDevice
{
    public:
        class DeviceALSA;

	public:
		AOQueryALSA();
		virtual ~AOQueryALSA();
		
		virtual bool queryNames();
		virtual bool queryDevice(int idx);
		
		virtual int defaultDeviceIndex();
		
	protected:
		
		QList<QSharedPointer<ALSAStreamParser> > m_streams;
		
		virtual void printError(const tchar *strR,const tchar *strE,int rc) const;
		
		virtual QVector<QString> listOfCards() const;
		virtual QString getPCMNameOfOutput(const QString& card);
};

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryALSA::DeviceALSA : public AOQueryDevice::Device
{
	public:
		DeviceALSA();
		DeviceALSA(const DeviceALSA& rhs);
		virtual ~DeviceALSA();
		
		virtual bool isSupported(const FormatDescription& desc) const;
		virtual const FormatsSupported& supported() const;
		
		virtual bool queryDevice(QSharedPointer<ALSAStreamParser>& pStreamInfo);
		
		virtual QString pcmDeviceName() const;
		
		virtual QVector<int> formatsFromDescription(const FormatDescription& desc) const;
		virtual bool canSupportFormat(snd_pcm_t *handle,tint fType,tint noChannels,tint frequency) const;
		
		virtual void print() const;
		
		static QString formatToString(int alsaFormat);

		virtual bool isDSDNative() const;
		virtual bool isDSDFrequencySupported(int freq, bool isNative);
		
	protected:
		
		static const int c_alsaFormats[23];
		
		FormatsSupported m_formats;
		QSet<int> m_nativeDSDRates;
		QSharedPointer<ALSAStreamParser> m_pStreamInfo;
		
		virtual void printError(const tchar *strE,const tchar *strR) const;
		virtual void printErrorOS(const tchar *strE,const tchar *strR,int err) const;
		
		virtual void copy(const AOQueryDevice::Device& rhs);
		
        virtual void querySupportedFormats(snd_pcm_t *handle);
		virtual bool hasFormat(snd_pcm_t *handle,const FormatDescription& desc) const;
		virtual bool descriptionFromFormat(int alsaFormat,int noChannels,int frequency,FormatDescription& desc) const;
		virtual void populateFrequencyAndChannelSets();
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
