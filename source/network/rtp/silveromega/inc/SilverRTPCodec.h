//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_SILVEROMEGA_SILVERRTPCODEC_H
#define __OMEGA_NETWORK_RTP_SILVEROMEGA_SILVERRTPCODEC_H
//-------------------------------------------------------------------------------------------

#include "network/inc/UDPRead.h"
#include "engine/inc/SequenceMemory.h"
#include "engine/silveromega/inc/SilverCodec.h"
#include "network/rtp/inc/Scheduler.h"
#include "network/rtp/silveromega/inc/VOggPackets.h"
#include "network/rtp/silveromega/inc/SilverSession.h"
#include "network/rtp/silveromega/inc/VTimeCalc.h"
#include "common/inc/DebugOutput.h"

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

class SilverRTPCodecDebugItem : public common::DebugOutputItem
{
    public:

        typedef enum
        {
            e_onData = 0,
            e_onNext
        } Type;

    public:
        SilverRTPCodecDebugItem(Type t);
        SilverRTPCodecDebugItem(const DebugOutputItem& item);

        Type& type();
        common::TimeStamp& time();

        virtual QString print() const;

    protected:

        Type m_type;
        common::TimeStamp m_time;

        virtual void copy(const DebugOutputItem& item);
};

//-------------------------------------------------------------------------------------------

class RTPSILVEROMEGA_EXPORT SilverRTPCodec : public engine::silveromega::SilverCodec
{
    public:
        Q_OBJECT

    public:
        SilverRTPCodec(QObject *parent = 0);
        virtual ~SilverRTPCodec();

        virtual bool open(const QString& name);
        virtual void close();

        virtual bool next(engine::AData& data);

        virtual common::TimeStamp length() const;

        virtual bool isComplete() const;

        virtual bool isRemote() const;
        virtual bool isBuffered(tfloat32& percent);
        virtual void resync();

    protected:

        VOggPackets *m_packets;
        Scheduler *m_scheduler;
        Session *m_session;
        QList<DataPacket> m_pList;
        bool m_completeFlag;
        bool m_bufferFlag;
        bool m_doneBufferFlag;
        VTimeCalc m_tCalc;
        common::TimeStamp m_timeBuffer;
        tint m_offset;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual bool readConfiguration();

        virtual bool getName(const QString& name,QString& sHost,tint& sPort,QString& rHost,tint& rPort,QString& fName) const;

        virtual bool openSession(const tchar *host,tint port,const tchar *rHost,tint rPort);
        virtual bool openSession(const QString& host,tint port,const QString& rHost,tint rPort);
        virtual void closeSession();

    protected slots:

        virtual void onData(unsigned int,void *);
        virtual void onComplete(network::rtp::Session *);
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

