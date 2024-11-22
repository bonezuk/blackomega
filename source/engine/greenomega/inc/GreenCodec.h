//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_GREENOMEGA_GREENCODEC_H
#define __OMEGA_ENGINE_GREENOMEGA_GREENCODEC_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/RData.h"
#include "engine/inc/Codec.h"
#include "engine/greenomega/inc/FLACFramework.h"
#include "engine/greenomega/inc/FLACFrame.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

class GREENOMEGA_EXPORT GreenCodecInitialize
{
	public:
		GreenCodecInitialize();
		~GreenCodecInitialize();
		
		static void start();
		static void end();
		
	protected:
	
		static GreenCodecInitialize *m_instance;
};

//-------------------------------------------------------------------------------------------

class GREENOMEGA_EXPORT GreenCodec : public engine::Codec
{
	public:
		Q_OBJECT
		
	public:
		GreenCodec(QObject *parent = 0);
		virtual ~GreenCodec();
		
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
		
		FLACFramework *m_framework;
		FLACFrame *m_frame;
		
		tint m_state;
		common::TimeStamp m_time;
		tint m_outOffset;
		tint m_outLen;

		common::BIOBufferedStream *m_cachedFile;
		engine::File *m_file;
		
		virtual void printError(const tchar *strR,const tchar *strE) const;
		
		virtual void setPartDataType(RData::Part& part);
};

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
