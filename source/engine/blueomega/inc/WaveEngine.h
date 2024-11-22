//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BLUEOMEGA_WAVEENGINE_H
#define __OMEGA_ENGINE_BLUEOMEGA_WAVEENGINE_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BIOStream.h"
#include "engine/inc/AData.h"
#include "engine/inc/Codec.h"
#include "engine/blueomega/inc/WaveInformation.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blueomega
{
//-------------------------------------------------------------------------------------------

#define DATA_ID BLUEOMEGA_ID('d','a','t','a')

//-------------------------------------------------------------------------------------------

class BLUEOMEGA_EXPORT WaveEngine : public engine::Codec
{
	public:
		Q_OBJECT
	
	public:
		WaveEngine(QObject *parent=0);
		virtual ~WaveEngine();
	
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
		
	protected:
	
		common::BIOStream *m_file;
		WaveInformation m_info;
		
		tint m_offset;
		tint m_dataSize;
		tint m_dataOffset;
		tint m_dataPosition;
		common::TimeStamp m_length;
		tubyte *m_buffer;
		tint m_bufferLength;
		common::TimeStamp m_currentTime;
		bool m_completeFlag;
		CodecDataType m_outputFormatType;
		
		virtual void printError(const tchar *strR,const tchar *strE) const;
		
		virtual tuint32 readChunkHeader(tint& size);
		virtual bool skipChunk(tint size);
		
		virtual void setupBuffer(const AData& data);

		virtual tint intFromMemory(tchar *mem) const;
		virtual tint shortFromMemory(tchar *mem) const;
		
		virtual void blankChannels(sample_t *dst,tint noSamples);
		virtual void blankChannelsFloat(sample_t *dst,tint noSamples);
		virtual void blankChannelsInt16Bit(sample_t *dst,tint noSamples);
		virtual void blankChannelsInt24Bit(sample_t *dst,tint noSamples);
		virtual void blankChannelsInt32Bit(sample_t *dst,tint noSamples);

		virtual void copyFrom8Bit(tubyte *src,sample_t *dst,tint noSamples);

		virtual void copyFromLE16Bit(tubyte *src,sample_t *dst,tint noSamples);
		virtual void copyFromLE24Bit(tubyte *src,sample_t *dst,tint noSamples);
		virtual void copyFromLE32Bit(tubyte *src,sample_t *dst,tint noSamples);
		virtual void copyFromLE32Float(tubyte *src,sample_t *dst,tint noSamples);

		virtual void copyFromBE16Bit(tubyte *src,sample_t *dst,tint noSamples);
		virtual void copyFromBE24Bit(tubyte *src,sample_t *dst,tint noSamples);
		virtual void copyFromBE32Bit(tubyte *src,sample_t *dst,tint noSamples);
		virtual void copyFromBE32Float(tubyte *src,sample_t *dst,tint noSamples);
		
		virtual void setPartDataType(RData::Part& part);
};

//-------------------------------------------------------------------------------------------
} // namespace blueomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

