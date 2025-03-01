//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_DSD_DSDCODEC_H
#define __OMEGA_ENGINE_DSD_DSDCODEC_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/Codec.h"
#include "engine/dsdomega/inc/DSFFileReader.h"
#include "engine/dsdomega/inc/DSDIFFFileReader.h"
#include "engine/dsdomega/inc/DSD2PCMConverter.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace dsd
{
//-------------------------------------------------------------------------------------------

/* DSD over PCM notes
   Although 24-bit and 32-bit depth encoding produces exactly the same data
   the difference lies in the dataTypesSupported return method in reporting
   which Int32 or Int24 type is returned. This is done such that a 32-bit DAC
   maps to a Int32 type and 24-bit DAC maps to a Int24 type. If the DAC has no
   support for these integer types specifically then DoP is not available on the DAC.
   Secondly, if differences are found then such PCM encoding logic is contained to the
   codec only.
   
   The PCM sample rate is set to DSD rate / 16. 16-bits per integer sample.
   DSD64   -  2822400Hz =  176400Hz PCM (44.1kHz)
   DSD64   -  3072000Hz =  192000Hz PCM (48KHz)
   DSD128  -  5644800Hz =  352800Hz PCM (44.1kHz)
   DSD128  -  6144000Hz =  384000Hz PCM (48KHz)
   DSD256  - 11289600Hz =  705600Hz PCM (44.1kHz)
   DSD256 -  12288000Hz =  768000Hz PCM (48KHz)
   DSD512  - 22579200Hz = 1411200Hz PCM (44.1kHz)
   DSD512  - 24576000Hz = 1536000Hz PCM (48KHz)
   DSD1024 - 45158400Hz = 2822400Hz PCM (44.1kHz)
   DSD1024 - 49152000Hz = 3072000Hz PCM (48KHz)
*/

class DSDOMEGA_EXPORT DSDCodec : public engine::Codec
{
	public:
		Q_OBJECT
	
	public:
		DSDCodec(QObject *parent=0);
		virtual ~DSDCodec();
	
		virtual bool open(const QString& name);
		virtual void close();
		
		virtual bool init();
		virtual bool next(AData& data);
		
		virtual bool isSeek() const;
		virtual bool seek(const common::TimeStamp& v);

		virtual bool isComplete() const;
		
		virtual bool isRemote() const;
		virtual bool isBuffered(tfloat32& percent);
		
		virtual tint bitrate() const;
		virtual tint frequency() const;
		virtual tint noChannels() const;
		virtual common::TimeStamp length() const;
		
		virtual CodecDataType dataTypesSupported() const;
		virtual bool setDataTypeFormat(CodecDataType type);
		
		virtual bool setOutputPCM(tint pcmFrequency);
		
		virtual bool isLSB() const;
		virtual bool isMSB() const;
		
	protected:
	
		common::BIOStream *m_file;
		DSDFileReader *m_dsdFileHandler;
		QVector<QByteArray> m_inBufferList;
		tint m_inSampleOffset;
		tint m_inBlockNumber;
		
		// The output frequency of PCM data. Set to zero to disable.
		tint m_pcmFrequency;
		QList<QSharedPointer<DSD2PCMConverter> > m_pcmConverters;
		QList<QByteArray> m_pcmBufferList;
		tint m_pcmSampleOffset;
		bool m_readComplete;
		
		// 0 = No DSD over PCM, 1 = DSD over PCM (24-bit), 2 = DSD over PCM (32-bit)
		tint m_isDSDOverPCM;
		tint m_markerIncr;
		
		virtual void printError(const tchar *strR,const tchar *strE) const;
		
		void setupDSFBuffers();
		void freeDSFBuffers();
		bool readInNextDSFBlock();
		tint64 bitAtInDSF(tint blockIdx, tint offset) const;
		common::TimeStamp timeAtInDSF(tint blockIdx, tint offset);
		tint currentBlockLength();
		
		bool initPCMOutput();
		bool nextPCMOutput(RData& rData);
		bool nextDSDOutput(RData& rData);
		
		bool writeDSDOutputNative(RData& rData, tint& pos);
		bool writeDSDOutputOverPCM(RData& rData, tint& pos);
};

//-------------------------------------------------------------------------------------------
} // namespace dsd
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
