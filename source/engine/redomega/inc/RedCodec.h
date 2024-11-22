//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_REDOMEGA_REDCODEC_H
#define __OMEGA_ENGINE_REDOMEGA_REDCODEC_H
//-------------------------------------------------------------------------------------------

#include "engine/redomega/inc/ALACSequence.h"
#include "engine/redomega/inc/ALACDecoder.h"
#include "common/inc/BIOBufferedStream.h"
#include "engine/inc/IOFile.h"
#include "engine/inc/Codec.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

class REDOMEGA_EXPORT RedCodecInitialize
{
	public:
		RedCodecInitialize();
		~RedCodecInitialize();
		
		static void start();
		static void end();
		
	protected:
	
		static RedCodecInitialize *m_instance;
};

//-------------------------------------------------------------------------------------------

class REDOMEGA_EXPORT RedCodec : public engine::Codec
{
	public:
		Q_OBJECT
		
	public:
		RedCodec(QObject *parent = 0);
		virtual ~RedCodec();
		
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
	
		common::BIOBufferedStream *m_fileStream;
		IOFile *m_file;
		
		ALACContainer *m_container;
		ALACDecoder *m_decoder;
		
		sample_t *m_outBuffer;
		tint m_outBufferSize;
		tint m_currentFrame;
		
		tint m_state;
		common::TimeStamp m_time;
		tint m_outOffset;
		tint m_outLen;
		
		CodecDataType m_outputFormatType;
		
		virtual void printError(const tchar *strR,const tchar *strE) const;
		
		virtual void setPartDataType(RData::Part& part);
};

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

