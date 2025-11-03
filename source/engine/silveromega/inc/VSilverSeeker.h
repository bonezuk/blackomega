//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVERSEEKER_H
#define __OMEGA_ENGINE_SILVEROMEGA_VSILVERSEEKER_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BOQueueTree.h"
#include "common/inc/TimeStamp.h"
#include "engine/inc/BitStream.h"
#include "engine/inc/Sequence.h"
#include "engine/silveromega/inc/SilverOmegaDLL.h"
#include "engine/silveromega/inc/VSilverContainer.h"
#include "engine/silveromega/inc/VSilverWindow.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

typedef struct s_OggPageHeader
{
    tint version;
    bool continued;
    bool first;
    bool last;
    tuint64 position;
    tuint serial;
    tuint pageNo;
    tuint checksum;
    tuint noSegments;
    tubyte segmentTable[256];
} OggPageHeader;

//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT VSilverSeeker
{
    public:
        VSilverSeeker(VSilverContainer *container);
        ~VSilverSeeker();
        
        bool init(const tchar *filename);
        bool init(const QString& filename);
        
        common::TimeStamp totalTime() const;
        
        bool seek(common::TimeStamp& seekTime,common::TimeStamp& actualTime,tuint& offset);
        
        tint offset() const;
        tint bitrate() const;
        
    protected:
    
        VSilverContainer *m_container;
        engine::File *m_file;
        engine::Bitstream *m_bitstream;
        tint m_sequenceNo;
        tuint m_serialNo;
        tuint m_firstOffset;
        tuint m_lastOffset;
        tuint m_firstPCM;
        tuint m_lastPCM;
        
        void printError(const tchar *strR,const tchar *strE) const;
        
        bool forwardSequenceSearch(tuint64& total,tint seqNo=0);
        
        bool readHeader(OggPageHeader *hdr,tuint64& total,tint seqNo=0);
        
        bool readPage(OggPageHeader *hdr,common::BOQueueTree<engine::Sequence *>& list);
        bool readPage(OggPageHeader *hdr,common::BOQueueTree<engine::Sequence *>& list,tuint64& total);
        
        bool checkCodecHeader(engine::Sequence *seq);
        
        void clearSequence(common::BOQueueTree<engine::Sequence *>& list);
        
        bool packetBlocksize(engine::Sequence *seq,tint& size);
        bool calcPageBlocksize(OggPageHeader *hdr,common::BOQueueTree<engine::Sequence *>& list,tuint64& total);
        
        bool readLastHeader(OggPageHeader *hdr,tuint& offset);
        
        bool calcLastPCM(tuint& offset,tuint& lastPCM);
        
        bool getSeekData();
        
        bool getPosition(tuint offset,tuint& position,tuint& actualOffset);
        bool seekPosition(tuint position,tuint& offset,tuint& actualOffset);
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

