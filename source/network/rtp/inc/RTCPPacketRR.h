//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_RTCPPACKETRR_H
#define __OMEGA_NETWORK_RTP_RTCPPACKETRR_H
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

class NETWORKRTP_EXPORT RTCPPacketRR : public RTCPPacket
{
    public:
        RTCPPacketRR(const Session& session);
        virtual ~RTCPPacketRR();

        virtual tint parse(NetArraySPtr mem,tint offset);
        virtual bool packet(NetArraySPtr mem);

        virtual tuint32 sessionID() const;
        void sessionID(tuint32 x);

        QList<RTCPReportBlock>& blockList();
        const QList<RTCPReportBlock>& blockList() const;

    protected:

        tint m_sessionID;
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

