//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_DSD_DSDIFFFILEREADER_H
#define __OMEGA_ENGINE_DSD_DSDIFFFILEREADER_H
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

#define DSDOMEGA_BE_ID(d,c,b,a) (((static_cast<tuint32>(static_cast<tint32>(d)) << 24) & 0xff000000) | \
                              ((static_cast<tuint32>(static_cast<tint32>(c)) << 16) & 0x00ff0000) | \
                              ((static_cast<tuint32>(static_cast<tint32>(b)) <<  8) & 0x0000ff00) | \
                              ((static_cast<tuint32>(static_cast<tint32>(a))      ) & 0x000000ff))


#define FORMDSD_CHUNK_ID     DSDOMEGA_BE_ID('F', 'R', 'M', '8')
#define FVER_CHUNK_ID        DSDOMEGA_BE_ID('F', 'V', 'E', 'R')
#define SND_CHUNK_ID         DSDOMEGA_BE_ID('S', 'N', 'D', ' ')
#define FRAMESAMPLE_CHUNK_ID DSDOMEGA_BE_ID('F', 'S', ' ', ' ')
#define CHANNELS_CHUNK_ID    DSDOMEGA_BE_ID('C', 'H', 'N', 'L')
#define COMPRESS_CHUNK_ID    DSDOMEGA_BE_ID('C', 'M', 'P', 'R')
#define DSDDATA_CHUNK_ID     DSDOMEGA_BE_ID('D', 'S', 'D', ' ')
#define PROP_CHUNK_ID        DSDOMEGA_BE_ID('P', 'R', 'O', 'P')

#define CHMAP_SLEFT_ID  DSDOMEGA_BE_ID('S', 'L', 'F', 'T') // 'SLFT' : stereo left
#define CHMAP_SRIGHT_ID DSDOMEGA_BE_ID('S', 'R', 'G', 'T') // 'SRGT' : stereo right
#define CHMAP_MLEFT_ID  DSDOMEGA_BE_ID('M', 'L', 'F', 'T') // 'MLFT' : multi-channel left
#define CHMAP_MRIGHT_ID DSDOMEGA_BE_ID('M', 'R', 'G', 'T') // 'MRGT' : multi-channel right
#define CHMAP_CENTER_ID DSDOMEGA_BE_ID('C', ' ', ' ', ' ') // 'C   ' : multi-channel center
#define CHMAP_LFE_ID    DSDOMEGA_BE_ID('L', 'F', 'E', ' ') // 'LFE ' : multi-channel low frequency enhancement
#define CHMAP_RLEFT_ID  DSDOMEGA_BE_ID('L', 'S', ' ', ' ') // 'LS  ' : multi-channel left surround
#define CHMAP_RRIGHT_ID DSDOMEGA_BE_ID('R', 'S', ' ', ' ') // 'RS  ' : multi-channel right surround


//-------------------------------------------------------------------------------------------

class DSDOMEGA_EXPORT DSDIFFFileReader : public DSDFileReader
{
	public:
		const int c_channelBlockSize = 4096;

	public:
		DSDIFFFileReader(common::BIOStream *ioFile);
		virtual ~DSDIFFFileReader();
		
		virtual bool parse();
		
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
		
	protected:
	
		tint m_frequency;
		tint m_noChannels;
		tint m_channelMap[8];
		tint64 m_dataStartOffset;
		tint64 m_dataLength;
		QByteArray m_inputData;
		
		virtual void printError(const tchar *strR, const tchar *strE) const;
		virtual tuint32 readHeader(tint64& len);
		virtual bool parseFormDSD(tint64 formLen);
		virtual bool isVersionValid(tint64 formLen);
		virtual bool parseProperty(tint64 formLen);
		virtual bool readSampleRate(tint64 formLen);
		virtual void defaultChannelMap();
		virtual int channelIndexFromID(tuint32 chID);
		virtual bool readChannelMap(tint64 formLen);
		virtual bool isCompressionValid(tint64 formLen);
		virtual void parseDSDData(tint64 formLen);
		virtual void reset();
};

//-------------------------------------------------------------------------------------------
} // namespace dsd
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
