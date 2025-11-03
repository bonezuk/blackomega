//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_SOURCESTATE_H
#define __OMEGA_NETWORK_RTP_SOURCESTATE_H
//-------------------------------------------------------------------------------------------

#include "network/inc/UDPRead.h"
#include "common/inc/TimeStamp.h"
#include "network/rtp/inc/Scheduler.h"
#include "network/rtp/inc/RTPPacket.h"
#include "network/rtp/inc/DataPacket.h"
#include "network/rtp/inc/RTCPPacketSR.h"
#include "common/inc/DebugOutput.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

#define RTP_SEQ_MOD (1 << 16)

const tint c_seqMaxDropout = 3000;
const tint c_seqMaxMisorder = 100;
const tint c_seqMinSequential = 2;

const tuint32 c_packetProcessThreshold = 3;

//-------------------------------------------------------------------------------------------

class SourceStateDebugItem : public common::DebugOutputItem
{
    public:
        SourceStateDebugItem();
        SourceStateDebugItem(const common::DebugOutputItem& item);

        tuint32& sourceID();
        tuint16& sequenceNo();
        common::TimeStamp& receive();
        common::TimeStamp& time();

        virtual QString print() const;

    protected:

        tuint32 m_sourceID;
        tuint16 m_sequenceNo;
        common::TimeStamp m_receive;
        common::TimeStamp m_time;

        virtual void copy(const common::DebugOutputItem& item);
};

//-------------------------------------------------------------------------------------------

class NETWORKRTP_EXPORT SourceState
{
    public:

        typedef enum
        {
            e_Process = 0,        // Process all listed packet and this one.
            e_Drop,                // Drop all listed packets and queue this one.
            e_Hold                // Place this one on end of listed packets.
        } UpdateState;

    public:
        SourceState(Session& session,tuint32 sourceID);
        ~SourceState();

        tuint32 ID() const;
        tuint32 cycles() const;

        void init(RTPPacketSPtr& p);

        bool processPacket(RTPPacketSPtr& p,QList<DataPacket>& pList);

        void onSR(const RTCPPacketSR& r);

        void report(RTCPReportBlock& block);

        const common::TimeStamp& startClock() const;
        const common::TimeStamp& randomClock() const;

        void resync();

    protected:

        Session& m_session;
        tuint32 m_sourceID;        // source SSRC.

        QList<RTPPacketSPtr> m_packetList;

        tuint16 m_maxSeq;          // highest sequence number seen
        tuint32 m_cycles;          // shifted count of sequence number cycles
        tuint32 m_baseSeq;         // base sequence number
        tuint32 m_badSeq;          // last 'bad' sequence number + 1
        tuint32 m_probation;       // sequence packets till source is valid
        tuint32 m_received;        // packets received
        tuint32 m_expectedPrior;   // packet expected at last interval
        tuint32 m_receivedPrior;   // packet received at last interval

        common::TimeStamp m_transit;            // relative trans time for previous packet
        tfloat64 m_jitter;                      // estimated jitter

        tuint32 m_lastSeqProcessed;

        common::TimeStamp m_lastSRTimeStamp;
        common::TimeStamp m_lastSRRecieve;

        common::TimeStamp m_startClockTime;
        common::TimeStamp m_randomTimeOffset;

        void printError(const tchar *strR,const tchar *strE) const;

        UpdateState update(RTPPacketSPtr& p);

        void initSequence(tuint16 seq);

        bool packetPosition(const QList<RTPPacketSPtr>& list,RTPPacketSPtr& p,tint& pos);
};

//-------------------------------------------------------------------------------------------

typedef SourceState* SourceStatePtr;
typedef QSharedPointer<SourceState> SourceStateSPtr;

//-------------------------------------------------------------------------------------------

inline tuint32 SourceState::ID() const
{
    return m_sourceID;
}

//-------------------------------------------------------------------------------------------

inline tuint32 SourceState::cycles() const
{
    return m_cycles;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& SourceState::startClock() const
{
    return m_startClockTime;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& SourceState::randomClock() const
{
    return m_randomTimeOffset;
}

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

