//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_RTCPREPORTBLOCK_H
#define __OMEGA_NETWORK_RTP_RTCPREPORTBLOCK_H
//-------------------------------------------------------------------------------------------

#include "network/inc/UDPRead.h"
#include "common/inc/TimeStamp.h"
#include "network/rtp/inc/NetworkRTPDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

class NETWORKRTP_EXPORT RTCPReportBlock
{
    public:
        RTCPReportBlock();
        RTCPReportBlock(const RTCPReportBlock& rhs);
        virtual ~RTCPReportBlock();
                
        const RTCPReportBlock& operator = (const RTCPReportBlock& rhs);
                
        tuint32 sessionID() const;
        void sessionID(tuint32 x);
                
        tint fractionLost() const;
        void fractionLost(tint x);
                
        tint packetsLost() const;
        void packetsLost(tint x);
                
        tuint32 highestSequenceNo() const;
        void highestSequenceNo(tuint32 x);
                
        tuint32 interarrivalJitter() const;
        void interarrivalJitter(tuint32 x);
                
        const common::TimeStamp& lastSR() const;
        void lastSR(const common::TimeStamp& x);
                
        const common::TimeStamp& delayLastSR() const;
        void delayLastSR(const common::TimeStamp& x);
                
    protected:
            
        tuint32 m_sessionID;
                
        tint m_fractionLost;
        tint m_packetLost;
        tuint32 m_highestSequenceNo;
        tuint32 m_interarrivalJitter;
        common::TimeStamp m_lastSR;
        common::TimeStamp m_delayLastSR;
        
        void copy(const RTCPReportBlock& rhs);
};

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

