//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_RTCPPACKETSR_H
#define __OMEGA_NETWORK_RTP_RTCPPACKETSR_H
//-------------------------------------------------------------------------------------------

#include "network/rtp/inc/RTCPPacket.h"
#include "network/rtp/inc/RTCPReportBlock.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

class NETWORKRTP_EXPORT RTCPPacketSR : public RTCPPacket
{
    public:
        RTCPPacketSR(const Session& session);
        virtual ~RTCPPacketSR();

        virtual tint parse(NetArraySPtr mem,tint offset);
        virtual bool packet(NetArraySPtr mem);

        virtual tuint32 sessionID() const;
        void sessionID(tuint32 x);

        const common::TimeStamp& time() const;
        void time(const common::TimeStamp& x);

        const common::TimeStamp& rtpTime() const;
        void rtpTime(const common::TimeStamp& x);

        tuint32 senderPacketCount() const;
        void senderPacketCount(tuint32 x);

        tuint32 senderOctetCount() const;
        void senderOctetCount(tuint32 x);

        QList<RTCPReportBlock>& blockList();
        const QList<RTCPReportBlock>& blockList() const;

    protected:

        tuint32 m_sessionID;
        common::TimeStamp m_time;
        common::TimeStamp m_rtpTime;
        tuint32 m_senderPacketCount;
        tuint32 m_senderOctetCount;

        QList<RTCPReportBlock> m_blocks;

        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

