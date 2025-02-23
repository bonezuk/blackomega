//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_DSD_DSFFILEREADER_H
#define __OMEGA_ENGINE_DSD_DSFFILEREADER_H
//-------------------------------------------------------------------------------------------

#include "engine/dsdomega/inc/DSDFileReader.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace dsd
{
//-------------------------------------------------------------------------------------------

class DSDOMEGA_EXPORT DSFFileReader : public DSDFileReader
{
	public:
		DSFFileReader(common::BIOStream *ioFile);
		virtual ~DSFFileReader();
		
		virtual bool parse();
		
		virtual tint channelInterleaveType() const;
		virtual tint numberOfChannels() const;
		virtual tint frequency() const;
		virtual bool isLSB() const;
		virtual bool isMSB() const;
		virtual tint bitsPerSample() const;
		virtual tint channelBlockSize() const;
		virtual tint64 totalSamples() const;
		
		virtual bool data(int blockIdx, QByteArray& arr, bool isBlockSize = false);
		virtual bool data(int blockIdx, int channelIdx, QByteArray& arr, bool isBlockSize = false);
		virtual tint64 numberOfBlocks() const;
		
		virtual bool isMetadata() const;
		virtual tint64 metaOffset() const;
		
	protected:
		tint m_numberOfChannels;
		tint m_frequency;
		tint m_bitsPerSample;
		tint m_channelInterleaveType;
		tint m_channelBlockSize;
		tint64 m_totalSamples;
		tint64 m_dataStartOffset;
		tint64 m_dataChunkSize;
		tint64 m_metaDataOffset;
		
		virtual bool parseDSDChunk();
		virtual bool parseFMTChunk();
		virtual bool parseDataChunk();
		virtual bool seekFileToChunkID(tuint32 chunkID, tint64& offset);
};

//-------------------------------------------------------------------------------------------
} // namespace dsd
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
