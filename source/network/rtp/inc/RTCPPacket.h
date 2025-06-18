//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_RTCPPACKET_H
#define __OMEGA_NETWORK_RTP_RTCPPACKET_H
//-------------------------------------------------------------------------------------------

#include "network/rtp/inc/RTPPacket.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

class NETWORKRTP_EXPORT RTCPPacket
{
    public:

        typedef QSharedPointer<RTCPPacket> RTCPPacketSPtr;

        typedef enum
        {
            e_SenderReport = 0,
            e_RecieverReport,
            e_SourceDescription,
            e_Goodbye,
            e_Application,
            e_Unknown
        } Type;
        
    public:
        RTCPPacket(const Session& session,Type t);
        virtual ~RTCPPacket();
        
        static bool isPacket(NetArraySPtr mem,tint offset = 0);
        static Type getPacketType(NetArraySPtr mem,tint offset = 0);
        static tint generate(const Session& session,QVector<RTCPPacketSPtr>& pVector,NetArraySPtr mem,tint offset = 0);
        static void append(RTCPPacketSPtr p,NetArraySPtr mem);
        static tint length(NetArraySPtr mem,tint offset = 0);
        
        virtual Type type() const;
        virtual tint parse(NetArraySPtr mem,tint offset) = 0;
        virtual bool packet(NetArraySPtr mem) = 0;
        
        virtual tuint32 sessionID() const = 0;
        
    protected:
    
        const Session& m_session;
        Type m_type;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------

typedef RTCPPacket* RTCPPacketPtr;
typedef QSharedPointer<RTCPPacket> RTCPPacketSPtr;

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

