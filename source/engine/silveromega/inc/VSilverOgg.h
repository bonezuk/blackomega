//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVEROGG_H
#define __OMEGA_ENGINE_SILVEROMEGA_VSILVEROGG_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BOQueueTree.h"
#include "engine/silveromega/inc/SilverOmegaDLL.h"
#include "engine/silveromega/inc/VSilverInfoObject.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT VSilverOgg
{
    public:
        VSilverOgg(engine::Bitstream *bs);
        ~VSilverOgg();

        engine::Sequence *next();

        bool seek(tint offset);

    protected:

        typedef struct s_OHeader
        {
            tint headerLength;
            tint bodyLength;
            tint noPackets;
            tint packets[256];
        } OHeader;

        engine::Bitstream *m_bitstream;

        tint m_sequenceNo;

        common::BOQueueTree<engine::Sequence *> m_packets;

        tint m_bookmark;
        tint m_streamID;

        engine::Sequence *m_pSeq;

        tint m_bkLast;

        void printError(const tchar *strR,const tchar *strE) const;

        bool sequenceSearch();

        bool nextPage(engine::Sequence *seq,tint bkStart) const;

        bool generatePackets(engine::Sequence *seq,tint bkStart,const OHeader *hdr,bool prevFlag);

        bool buildPacketSequence(tint bkFrom,tint startIndex,tint length,bool prevFlag);

        bool readHeader(engine::Sequence *seq,tint bkStart,OHeader *hdr);

        bool checksumPage(engine::Sequence *seq,tint bkStart,const OHeader *hdr) const;

        bool doPage();

        void clearList();
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

