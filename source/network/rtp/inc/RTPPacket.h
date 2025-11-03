//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_RTPPACKET_H
#define __OMEGA_NETWORK_RTP_RTPPACKET_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"
#include "network/inc/NetMemory.h"
#include "network/rtp/inc/DataPacket.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

class Session;

//-------------------------------------------------------------------------------------------

class NETWORKRTP_EXPORT RTPPacket : public QObject
{
    public:
        Q_OBJECT

    public:
        RTPPacket(Session& session,QObject *parent = 0);
        virtual ~RTPPacket();

        // header functions.
        virtual tint version() const;
        virtual bool padding() const;
        virtual bool extension() const;
        virtual bool marker() const;
        virtual tint payloadType() const;
        virtual tuint32 sequenceNo() const;
        virtual common::TimeStamp time() const;

        virtual tuint32 sourceID() const;
        virtual tuint32 contributingID(tint idx) const;
        virtual tint contributingNumber() const;

        virtual void setCycles(tuint32 x);

        virtual void buildHeader();

        virtual NetArraySPtr body();
        virtual void setBody(NetArraySPtr bPtr);

        virtual bool packet(NetArraySPtr mem);

        virtual tint parse(NetArraySPtr mem,tint offset);

    protected:

        Session& m_session;

        bool m_headerFlag;
        tuint32 m_sequenceNo;
        NetArray m_header;
        NetArraySPtr m_body;

        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------

typedef RTPPacket* RTPPacketPtr;
typedef QSharedPointer<RTPPacket> RTPPacketSPtr;

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

