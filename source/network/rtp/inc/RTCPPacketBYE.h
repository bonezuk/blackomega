//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_RTCPPACKETBYE_H
#define __OMEGA_NETWORK_RTP_RTCPPACKETBYE_H
//-------------------------------------------------------------------------------------------

#include "network/rtp/inc/RTCPPacket.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

class NETWORKRTP_EXPORT RTCPPacketBYE : public RTCPPacket
{
    public:
        RTCPPacketBYE(const Session& session);
        virtual ~RTCPPacketBYE();
        
        const QString& reason() const;
        void reason(const QString& x);
        
        QList<tuint32>& sessionList();
        const QList<tuint32>& sessionList() const;

        virtual tuint32 sessionID() const;

        virtual tint parse(NetArraySPtr mem,tint offset);
        virtual bool packet(NetArraySPtr mem);

    protected:
        
        QList<tuint32> m_sessionList;
        QString m_reason;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

