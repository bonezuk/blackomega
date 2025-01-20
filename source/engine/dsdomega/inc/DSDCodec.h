//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_DSD_DSDCODEC_H
#define __OMEGA_ENGINE_DSD_DSDCODEC_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/Codec.h"
#include "engine/dsdomega/inc/DSFFileReader.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace dsd
{
//-------------------------------------------------------------------------------------------

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
		
	protected:
	
		common::BIOStream *m_file;
		DSFFileReader *m_dsfHandler;
		QVector<QByteArray> m_inBufferList;
		tint m_inSampleOffset;
		tint m_inBlockNumber;
		
		virtual void printError(const tchar *strR,const tchar *strE) const;
		
		void setupDSFBuffers();
		void freeDSFBuffers();
		bool readInNextDSFBlock();
		tint64 bitAtInDSF(tint blockIdx, tint offset) const;
		common::TimeStamp timeAtInDSF(tint blockIdx, tint offset);
		tint currentBlockLength();
};

//-------------------------------------------------------------------------------------------
} // namespace dsd
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
