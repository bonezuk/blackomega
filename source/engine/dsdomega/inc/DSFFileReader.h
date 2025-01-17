//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_DSD_DSFFILEREADER_H
#define __OMEGA_ENGINE_DSD_DSFFILEREADER_H
//-------------------------------------------------------------------------------------------

#include <QByteArray>

#include "common/inc/BIOStream.h"
#include "engine/dsdomega/inc/DSDOmegaDLL.h"
#include "engine/violetomega/inc/IFFChunk.h"
#include "engine/inc/FormatType.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace dsd
{
//-------------------------------------------------------------------------------------------

#define DSDOMEGA_ID(a,b,c,d) (((static_cast<tuint32>(static_cast<tint32>(d)) << 24) & 0xff000000) | \
                              ((static_cast<tuint32>(static_cast<tint32>(c)) << 16) & 0x00ff0000) | \
                              ((static_cast<tuint32>(static_cast<tint32>(b)) <<  8) & 0x0000ff00) | \
                              ((static_cast<tuint32>(static_cast<tint32>(a))      ) & 0x000000ff))


#define DSD_CHUNK_ID DSDOMEGA_ID('D', 'S', 'D', ' ')
#define FMT_CHUNK_ID DSDOMEGA_ID('f', 'm', 't', ' ')
#define DATA_CHUNK_ID DSDOMEGA_ID('d', 'a', 't', 'a')

//-------------------------------------------------------------------------------------------

class DSDOMEGA_EXPORT DSFFileReader
{
	public:
		DSFFileReader(common::BIOStream *ioFile);
		virtual ~DSFFileReader();
		
		virtual bool parse();
		
		virtual tint numberOfChannels() const;
		virtual tint frequency() const;
		virtual bool isLSB() const;
		virtual bool isMSB() const;
		virtual tint bitsPerSample() const;
		virtual tint channelBlockSize() const;
		virtual tint64 totalSamples() const;
		
		virtual bool data(int blockIdx, int channelIdx, QByteArray& arr);
		virtual tint64 numberOfBlocks() const;
		
		virtual bool isMetadata() const;
		virtual tint64 metaOffset() const;
		
	private:
		common::BIOStream *m_ioFile;
		tint m_numberOfChannels;
		tint m_frequency;
		tint m_bitsPerSample;
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
