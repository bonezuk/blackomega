#include "network/rtp/inc/RTCPPacketSR.h"
#include "common/inc/Memory.h"
#include "network/rtp/inc/Session.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------
// RTCPPacketSR
//-------------------------------------------------------------------------------------------

RTCPPacketSR::RTCPPacketSR(const Session& session) : RTCPPacket(session,RTCPPacket::e_SenderReport),
    m_sessionID(),
    m_time(),
    m_rtpTime(),
    m_senderPacketCount(0),
    m_senderOctetCount(0),
    m_blocks()
{}

//-------------------------------------------------------------------------------------------

RTCPPacketSR::~RTCPPacketSR()
{}

//-------------------------------------------------------------------------------------------

void RTCPPacketSR::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "RTCPPacketSR::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

tint RTCPPacketSR::parse(NetArraySPtr mem,tint offset)
{
    tint i,done = -1,reportLen = length(mem,offset);

    if(reportLen>=28 && (offset + reportLen)<=mem->GetSize())
    {
        tint rCount;
        const tubyte *x = reinterpret_cast<const tubyte *>(mem->GetData());

        rCount = static_cast<tint>((x[offset] >> 3) & 0x1f);
        if((x[offset] & 0x03)==2 && x[offset+1]==0xc8)
        {
            if((28 + (rCount * 24)) <= reportLen)
            {
                m_sessionID = NetMemory::toInt(mem,offset + 4);
                m_time = NetMemory::toLong(mem,offset + 8);
                m_rtpTime = NetMemory::toInt(mem,offset + 16);
                m_senderPacketCount = NetMemory::toInt(mem,offset + 20);
                m_senderOctetCount = NetMemory::toInt(mem,offset + 24);

                x = &x[offset + 28];

                for(i=0;i<rCount;++i,x+=24)
                {
                    RTCPReportBlock b;
                    common::TimeStamp tA,tB;

                    b.sessionID(NetMemory::toInt(x,0));
                    b.fractionLost(static_cast<tint>(x[4]));
                    b.packetsLost(static_cast<tint>(NetMemory::toSInt24(x,5)));
                    b.highestSequenceNo(static_cast<tuint32>(NetMemory::toInt(x,8)));
                    b.interarrivalJitter(static_cast<tuint32>(NetMemory::toInt(x,12)));
                    tA = NetMemory::toInt(x,16);
                    b.lastSR(tA);
                    tB = NetMemory::toInt(x,20);
                    b.delayLastSR(tB);

                    m_blocks.append(b);
                }

                done = offset + reportLen;
            }
            else
            {
                printError("parse","Invalid RTCP sender report as length is less than expected");
            }
        }
        else
        {
            printError("parse","Invalid RTCP sender report");
        }
    }
    else
    {
        printError("parse","Invalid RTCP sender report as expected length is too short");
    }
    return done;
}

//-------------------------------------------------------------------------------------------

bool RTCPPacketSR::packet(NetArraySPtr mem)
{
    NetArray pMem;
    tint i,reportLen,bSize = (m_blocks.size() & 0x0000001f);
    tubyte *x;

    reportLen = 28 + (bSize * 24);
    pMem.SetSize(reportLen);
    x = reinterpret_cast<tubyte *>(pMem.GetData());

    x[0] = 0x02;
    x[0] |= (static_cast<tubyte>(bSize) << 3) & 0xf8;
    x[1] = 0xc8;
    NetMemory::fromShort(x,2,static_cast<tuint16>((reportLen / 4) - 1));
    NetMemory::fromInt(x,4,m_sessionID);
    NetMemory::fromLong(x,8,static_cast<tuint64>(m_time));
    NetMemory::fromInt(x,16,static_cast<tuint32>(m_rtpTime));
    NetMemory::fromInt(x,20,m_senderPacketCount);
    NetMemory::fromInt(x,24,m_senderOctetCount);

    x = &x[28];
    for(i=0;i<bSize;++i,x+=24)
    {
        const RTCPReportBlock& b = m_blocks.at(i);
        NetMemory::fromInt(x,0,b.sessionID());
        x[4] = static_cast<tubyte>(b.fractionLost());
        NetMemory::fromSInt24(x,5,static_cast<tint32>(b.packetsLost()));
        NetMemory::fromInt(x,8,b.highestSequenceNo());
        NetMemory::fromInt(x,12,b.interarrivalJitter());
        NetMemory::fromInt(x,16,static_cast<tuint32>(b.lastSR()));
        NetMemory::fromInt(x,20,static_cast<tuint32>(b.delayLastSR()));
    }

    mem->Append(pMem);

    return true;
}

//-------------------------------------------------------------------------------------------

tuint32 RTCPPacketSR::sessionID() const
{
    return m_sessionID;
}

//-------------------------------------------------------------------------------------------

void RTCPPacketSR::sessionID(tuint32 x)
{
    m_sessionID = x;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& RTCPPacketSR::time() const
{
    return m_time;
}

//-------------------------------------------------------------------------------------------

void RTCPPacketSR::time(const common::TimeStamp& x)
{
    m_time = x;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& RTCPPacketSR::rtpTime() const
{
    return m_rtpTime;
}

//-------------------------------------------------------------------------------------------

void RTCPPacketSR::rtpTime(const common::TimeStamp& x)
{
    m_rtpTime = x;
}

//-------------------------------------------------------------------------------------------

tuint32 RTCPPacketSR::senderPacketCount() const
{
    return m_senderPacketCount;
}

//-------------------------------------------------------------------------------------------

void RTCPPacketSR::senderPacketCount(tuint32 x)
{
    m_senderPacketCount = x;
}

//-------------------------------------------------------------------------------------------

tuint32 RTCPPacketSR::senderOctetCount() const
{
    return m_senderOctetCount;
}

//-------------------------------------------------------------------------------------------

void RTCPPacketSR::senderOctetCount(tuint32 x)
{
    m_senderOctetCount = x;
}

//-------------------------------------------------------------------------------------------

QList<RTCPReportBlock>& RTCPPacketSR::blockList()
{
    return m_blocks;
}

//-------------------------------------------------------------------------------------------

const QList<RTCPReportBlock>& RTCPPacketSR::blockList() const
{
    return m_blocks;
}

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
