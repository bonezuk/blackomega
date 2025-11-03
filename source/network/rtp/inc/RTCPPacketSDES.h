//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_RTCPPACKETSDES_H
#define __OMEGA_NETWORK_RTP_RTCPPACKETSDES_H
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

class NETWORKRTP_EXPORT SDESBlock
{
    public:
        SDESBlock();
        SDESBlock(const SDESBlock& rhs);
        virtual ~SDESBlock();

        const SDESBlock& operator = (const SDESBlock& rhs);

        tuint32 sessionID() const;
        void sessionID(tuint32 x);

        const QString& canonical() const;
        void canonical(const QString& x);

        const QString& user() const;
        void user(const QString& x);

        const QString& email() const;
        void email(const QString& x);

        const QString& phone() const;
        void phone(const QString& x);

        const QString& location() const;
        void location(const QString& x);

        const QString& tool() const;
        void tool(const QString& x);

        const QString& note() const;
        void note(const QString& x);

        const common::TimeStamp& lastReceived() const;
        void lastReceived(const common::TimeStamp& x);

    protected:

        tuint m_sessionID;
        QString m_canonical;
        QString m_user;
        QString m_email;
        QString m_phone;
        QString m_location;
        QString m_tool;
        QString m_note;

        common::TimeStamp m_lastReceived;

        virtual void copy(const SDESBlock& rhs);
};

//-------------------------------------------------------------------------------------------

class NETWORKRTP_EXPORT RTCPPacketSDES : public RTCPPacket
{
    public:
        RTCPPacketSDES(const Session& session);
        virtual ~RTCPPacketSDES();

        virtual tuint32 sessionID() const;

        virtual tint parse(NetArraySPtr mem,tint offset);
        virtual bool packet(NetArraySPtr mem);

        QList<SDESBlock>& blockList();
        const QList<SDESBlock>& blockList() const;

    protected:

        QList<SDESBlock> m_blocks;

        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

