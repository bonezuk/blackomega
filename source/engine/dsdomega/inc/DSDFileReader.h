//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_DSD_DSDFILEREADER_H
#define __OMEGA_ENGINE_DSD_DSDFILEREADER_H
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

const tubyte c_dsdIdleSample = 0x69;

//-------------------------------------------------------------------------------------------

#define DSDOMEGA_ID(a,b,c,d) (((static_cast<tuint32>(static_cast<tint32>(d)) << 24) & 0xff000000) | \
                              ((static_cast<tuint32>(static_cast<tint32>(c)) << 16) & 0x00ff0000) | \
                              ((static_cast<tuint32>(static_cast<tint32>(b)) <<  8) & 0x0000ff00) | \
                              ((static_cast<tuint32>(static_cast<tint32>(a))      ) & 0x000000ff))


#define DSD_CHUNK_ID DSDOMEGA_ID('D', 'S', 'D', ' ')
#define FMT_CHUNK_ID DSDOMEGA_ID('f', 'm', 't', ' ')
#define DATA_CHUNK_ID DSDOMEGA_ID('d', 'a', 't', 'a')

//-------------------------------------------------------------------------------------------

class DSDOMEGA_EXPORT DSDFileReader
{
	public:
		DSDFileReader(common::BIOStream *ioFile);
		virtual ~DSDFileReader();
		
		virtual bool parse() = 0;
		
		virtual tint numberOfChannels() const = 0;
		virtual tint frequency() const = 0;
		virtual bool isLSB() const = 0;
		virtual bool isMSB() const = 0;
		virtual tint bitsPerSample() const = 0;
		virtual tint channelBlockSize() const = 0;
		virtual tint64 totalSamples() const = 0;
		
		virtual bool data(int blockIdx, QByteArray& arr, bool isBlockSize = false) = 0;
		virtual bool data(int blockIdx, int channelIdx, QByteArray& arr, bool isBlockSize = false) = 0;
		virtual tint64 numberOfBlocks() const = 0;
		
	protected:
		common::BIOStream *m_ioFile;
};

//-------------------------------------------------------------------------------------------
} // namespace dsd
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
