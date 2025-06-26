//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_SESSION_H
#define __OMEGA_NETWORK_RTP_SESSION_H
//-------------------------------------------------------------------------------------------

#include "network/inc/UDPRead.h"
#include "network/inc/UDPWrite.h"

#include <QTimer>
#include <QDateTime>

#include "common/inc/Factory.h"
#include "network/rtp/inc/Scheduler.h"
#include "network/rtp/inc/DataPacket.h"
#include "network/rtp/inc/RTPPacket.h"
#include "network/rtp/inc/RTCPPacketRR.h"
#include "network/rtp/inc/RTCPPacketSR.h"
#include "network/rtp/inc/RTCPPacketAPP.h"
#include "network/rtp/inc/RTCPPacketBYE.h"
#include "network/rtp/inc/RTCPPacketSDES.h"
#include "network/rtp/inc/SourceState.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

class NETWORKRTP_EXPORT Session : public QObject
{
    public:
        Q_OBJECT

    public:
        Session(QObject *parent = 0);
        virtual ~Session();

        static common::TimeStamp clockTime();

        virtual bool isSource() const;

        virtual bool start(Scheduler *scheduler,const QString& resource,const QString& host,tint port,const QString& rHost,tint rPort);
        virtual bool start(Scheduler *scheduler,const QString& host,tint port,const QString& rHost,tint rPort);
        virtual void stop();

        // RTP packet header definitions for session data
        virtual bool marker() const;
        virtual tint payloadType() const;
        virtual tuint32 sequenceNo() const;
        virtual common::TimeStamp time() const;
        virtual common::TimeStamp time(tuint32 id) const;
        virtual common::TimeStamp rtime() const;
        virtual tuint32 sourceID() const;
        virtual tuint32 contributingID(tint idx) const;
        virtual tint contributingNumber() const;

        virtual bool isSupportedPayload(tint t) const;

        virtual const common::TimeStamp& startClockTime() const;
        virtual const common::TimeStamp& startClockTime(tuint32 srcID) const;
        virtual const common::TimeStamp& randomClockTime(tuint32 srcID) const;

        virtual tint getRTPBody(const RTPPacket& p,NetArraySPtr mem,tint offset,NetArraySPtr body) = 0;
        virtual bool unpackPacket(RTPPacketSPtr& p,QList<DataPacket>& pList) = 0;

        // checks the sender and reciever list for any timeout on the connections.
        virtual void onTimeout();

        virtual void resync();

    protected:
        Scheduler *m_scheduler;

        UDPRead *m_input;
        UDPWrite *m_output;

        QString m_rHost;
        tint m_rPort;

        bool m_source;

        common::TimeStamp m_time;
        common::TimeStamp m_startClockTime;
        common::TimeStamp m_randomTimeOffset;
        common::TimeStamp m_lastRecieveTime;

        tuint32 m_sequenceNo;

        RTPPacketSPtr m_currentPacket;
        bool m_completeFlag;
        bool m_byeFlag;
        tuint32 m_senderPacketCount;
        tuint32 m_senderDataCount;

        // this session's SSRC
        tuint32 m_sourceID;

        // session bandwidth parameter.
        tfloat64 m_sessionBW;

        // RTCP control variables
        // tc: the current time - computed via clockTime method
        common::TimeStamp m_tp;            // tp: the last time an RTCP packet was transmitted.
        common::TimeStamp m_tn;            // tn: the next scheduled transmission time of an RTCP packet
        // rtcp_bw: The target RTCP bandwidth, i.e, the total bandwidth that will be used for RTCP packets
        // by all members of this session, in octets per second. This will be a specified fraction of the
        // "session bandwidth" parameter supplied to the application at startup.
        tfloat64 m_rtcpBW;
        // avg_rtcp_size: The average compound RTCP packet, size in octets, over all RTCP packets sent and
        // received by this participant.
        tfloat64 m_avgRTCPSize;
        // initial: Flag that is true if the application has not yet sent an RTCP packet.
        bool m_initialRTCPFlag;

        QMap<tuint32,SDESBlock> m_memberSet;
        QSet<tuint32> m_senderSet;

        QTimer *m_timerData;
        QTimer *m_timerReport;

        // holds the source SSRC along with corrisponding report data.
        QMap<tuint32,SourceStateSPtr> m_sourceSessionMap;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual bool openNetwork(const QString& host,tint port);
        virtual void closeNetwork();

        virtual bool openResource(const QString& resource) = 0;
        virtual void closeResource() = 0;

        virtual bool generatePacketData(common::TimeStamp& t,RTPPacketSPtr p) = 0;

        virtual void startSession();
        virtual void stopSession();
        virtual bool send();
        virtual bool sendPacket();
        virtual bool compileNextPacket();

        virtual bool processPacket(RTPPacketSPtr& p,QList<DataPacket>& pList);

        virtual void scheduleReport();

        virtual void setTimer(QTimer *timer,tint msec);

        virtual void onSendSenderReport();
        virtual void onSendReceiverReport();
        virtual void onSendDescription();
        virtual void onSendBye();
        virtual void addCompoundDescription(NetArraySPtr mem);

        virtual common::TimeStamp rtcpInterval(bool rFlag = true);

        virtual void onRecieveSR(RTCPPacketSR& p);
        virtual void onRecieveRR(RTCPPacketRR& p);
        virtual void onRecieveAPP(RTCPPacketAPP& p);
        virtual void onRecieveBYE(RTCPPacketBYE& p);
        virtual void onRecieveSDES(RTCPPacketSDES& p);

        virtual tint members() const;
        virtual tint senders() const;
        virtual bool isMember(tuint32 id) const;
        virtual bool isSender(tuint32 id) const;

    Q_SIGNALS:
        void onComplete(network::rtp::Session *);
        void onData(unsigned int,void *);

    protected Q_SLOTS:
        virtual void onSendData();
        virtual void onSendReport();
        virtual void onRecieve(const QString& host,tint port,network::NetArraySPtr mem);
};

//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS(NETWORKRTP_EXPORT,SessionFactory,Session)

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
