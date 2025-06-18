//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_VIOLETOMEGA_IFFSOUNDCHUNK_H
#define __OMEGA_ENGINE_VIOLETOMEGA_IFFSOUNDCHUNK_H
//-------------------------------------------------------------------------------------------

#include "engine/violetomega/inc/IFFChunk.h"
#include "engine/violetomega/inc/IFFCommonChunk.h"
#include "common/inc/TimeStamp.h"
#include "engine/inc/AData.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace violetomega
{
//-------------------------------------------------------------------------------------------

#define IFF_SSND_ID IFF_ID('S','S','N','D')

//-------------------------------------------------------------------------------------------

class VIOLETOMEGA_EXPORT IFFSoundChunk : public IFFChunk
{
    public:
        IFFSoundChunk();
        virtual ~IFFSoundChunk();
        
        virtual void setCommon(IFFCommonChunkSPtr pCommon);
        
        virtual bool scan();
        
        virtual const tint& offset() const;
        virtual const tint& blockSize() const;
        
        virtual int numberOfSamples() const;
        
        virtual int read(sample_t *sampleMem, int noSamples, CodecDataType type);
        
        virtual bool seek(int idx);
        
        virtual int noOutChannels() const;
        
        virtual common::TimeStamp currentTime() const;
        
        virtual tint bitrate() const;

        virtual int bytesPerSample() const;
        
    protected:
        
        tint m_offset;
        tint m_blockSize;
        
        tint m_noChannels;
        tint m_noSampleFrames;
        tint m_sampleSize;
        tfloat64 m_sampleRate;
        IFFCommonChunk::PCMFormatType m_formatType;
        
        int m_currentIndexPosition;
        
        tbyte *m_readBlockMem;
        tint m_readBlockSize;
        
        virtual sample_t readSample(const tbyte *mem,int noBits);
        virtual tint16 readSampleInt16(const tbyte *mem, int noBits);
        virtual tint32 readSampleInt24(const tbyte *mem, int noBits);
        virtual tint32 readSampleInt32(const tbyte *mem, int noBits);
        
        virtual int indexPosition(int idx) const;
        virtual int bytesPerSampleBlock() const;
        virtual int bytesPerFrame() const;
        
        virtual int blocksPerSection() const;
        virtual int totalSampleBytes() const;
        
        virtual void setCurrentIndexPosition(int idx);
        virtual int currentIndexPosition();
        virtual int nextIndexPosition();

        template<class X> void sortOutputChannels(const X *in, X *out);
        void sortOutputChannelsSample(const sample_t* in, sample_t* out);
        
        virtual int readAsWhole(sample_t *sampleMem, int noSamples, CodecDataType type);
        virtual int readAsBlocks(sample_t *sampleMem, int noSamples, CodecDataType type);
};

typedef QSharedPointer<IFFSoundChunk> IFFSoundChunkSPtr;

//-------------------------------------------------------------------------------------------
} // namespace violetomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
