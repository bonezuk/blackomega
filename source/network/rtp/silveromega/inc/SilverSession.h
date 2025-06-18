//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_SILVEROMEGA_SILVERSESSION_H
#define __OMEGA_NETWORK_RTP_SILVEROMEGA_SILVERSESSION_H
//-------------------------------------------------------------------------------------------

#include "network/rtp/inc/Session.h"
#include "network/rtp/silveromega/inc/VOggPackets.h"
#include "network/rtp/silveromega/inc/VTimeCalc.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

class RTPSILVEROMEGA_EXPORT SilverSession : public Session
{
    public:
        Q_OBJECT
        
    public:
        SilverSession(QObject *parent = 0);
        virtual ~SilverSession();
        
        virtual tint getRTPBody(const RTPPacket& p,NetArraySPtr mem,tint offset,NetArraySPtr body);
        
        virtual bool unpackPacket(RTPPacketSPtr& p,QList<DataPacket>& pList);

        void setTCalc(VTimeCalc *calc);
        
    protected:
        
        VOggPackets m_packets;
        
        common::TimeStamp m_lastPacketTime;
        tint m_lastPacketOffset;
        NetArray m_lastPacket;
        VTimeCalc *m_tCalc;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        virtual bool openResource(const QString& resource);
        virtual void closeResource();
        
        virtual bool generatePacketData(common::TimeStamp& t,RTPPacketSPtr p);
        
        virtual bool buildPacket(common::TimeStamp& ts,NetArray& mem);
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
