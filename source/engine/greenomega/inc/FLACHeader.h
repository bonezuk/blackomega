//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_GREENOMEGA_FLACHEADER_H
#define __OMEGA_ENGINE_GREENOMEGA_FLACHEADER_H
//-------------------------------------------------------------------------------------------

#include "engine/greenomega/inc/FLACMetaBase.h"
#include "engine/greenomega/inc/FLACMetaStreamInfo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

class GREENOMEGA_EXPORT FLACHeader
{
    public:
    
        typedef enum
        {
            e_Independent = 0,
            e_LeftSide,
            e_RightSide,
            e_MidSide
        } ChannelAssignmentType;
        
        typedef enum
        {
            e_FrameNumber = 0,
            e_SampleNumber
        } FrameNumberType;
        
    public:
        FLACHeader(FLACMetaStreamInfo *sInfo);
        virtual ~FLACHeader();
        
        bool read(Sequence *seq);
        
        static tuint32 readUTF8Encoded32(Sequence *seq);
        static tuint32 readUTF8Encoded32(Sequence *seq,tubyte *mem,tint& len);
        static tuint64 readUTF8Encoded64(Sequence *seq);
        static tuint64 readUTF8Encoded64(Sequence *seq,tubyte *mem,tint& len);
        
        static tubyte crc8(tubyte *mem,tint len);
        
        const tint32& blockSize() const;
        const tint32& sampleRate() const;
        const tint32& channels() const;
        ChannelAssignmentType channelAssignment() const;
        const tint& bitsPerSample() const;
        const tuint32& frameNumber() const;
        const tuint64& sampleNumber() const;
        const tuint32& fixedBlockSize() const;
        const tuint32& nextFixedBlockSize() const;
        
        tint bookmarkStart() const;
        
    protected:
        
        FLACMetaStreamInfo *m_streamInfo;
        
        // samples per subframe
        tint m_blockSize;
        // sample rate in Hz
        tint m_sampleRate;
        // number of channels
        tint m_channels;
        // channel assignment
        ChannelAssignmentType m_channelAssignment;
        // sample resolution
        tint m_bitsPerSample;
        // frame numbering scheme
        FrameNumberType m_numberType;
        // frame number or sample number of first sample in frame
        tuint32 m_frameNumber;
        tuint64 m_sampleNumber;
        
        tuint32 m_fixedBlockSize;
        tuint32 m_nextFixedBlockSize;
        
        tint m_bkHeader;
        
        bool calcSampleNumber();
};

//-------------------------------------------------------------------------------------------

inline const tint32& FLACHeader::blockSize() const
{
    return m_blockSize;
}

//-------------------------------------------------------------------------------------------

inline const tint32& FLACHeader::sampleRate() const
{
    return m_sampleRate;
}

//-------------------------------------------------------------------------------------------

inline const tint32& FLACHeader::channels() const
{
    return m_channels;
}

//-------------------------------------------------------------------------------------------

inline FLACHeader::ChannelAssignmentType FLACHeader::channelAssignment() const
{
    return m_channelAssignment;
}

//-------------------------------------------------------------------------------------------

inline const tint& FLACHeader::bitsPerSample() const
{
    return m_bitsPerSample;
}

//-------------------------------------------------------------------------------------------

inline const tuint32& FLACHeader::frameNumber() const
{
    return m_frameNumber;
}

//-------------------------------------------------------------------------------------------

inline const tuint64& FLACHeader::sampleNumber() const
{
    return m_sampleNumber;
}

//-------------------------------------------------------------------------------------------

inline const tuint32& FLACHeader::fixedBlockSize() const
{
    return m_fixedBlockSize;
}

//-------------------------------------------------------------------------------------------

inline const tuint32& FLACHeader::nextFixedBlockSize() const
{
    return m_nextFixedBlockSize;
}

//-------------------------------------------------------------------------------------------

inline tint FLACHeader::bookmarkStart() const
{
    return m_bkHeader;
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

