//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_RTCPPACKETAPP_H
#define __OMEGA_NETWORK_RTP_RTCPPACKETAPP_H
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

class NETWORKRTP_EXPORT RTCPPacketAPP : public RTCPPacket
{
    public:
        RTCPPacketAPP(const Session& session);
        virtual ~RTCPPacketAPP();
        
        tint subType() const;
        void subType(tint x);
        
        virtual tuint32 sessionID() const;
        void sessionID(tuint32 x);
        
        const QString& name() const;
        void name(const QString& x);
        
        virtual tint parse(NetArraySPtr mem,tint offset);
        virtual bool packet(NetArraySPtr mem);
        
    protected:
    
        tint m_subType;
        tuint32 m_sessionID;
        QString m_name;
    
        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

