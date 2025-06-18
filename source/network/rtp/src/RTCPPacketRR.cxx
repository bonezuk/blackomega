#include "network/rtp/inc/RTCPPacketRR.h"
#include "network/rtp/inc/Session.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

RTCPPacketRR::RTCPPacketRR(const Session& session) : RTCPPacket(session,e_RecieverReport),
    m_sessionID(0),
    m_blocks()
{}

//-------------------------------------------------------------------------------------------

RTCPPacketRR::~RTCPPacketRR()
{}

//-------------------------------------------------------------------------------------------

void RTCPPacketRR::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "RTCPPacketRR::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

tint RTCPPacketRR::parse(NetArraySPtr mem,tint offset)
{
    tint i,done = -1,reportLen = length(mem);
    
    if(reportLen>=8 && (offset + reportLen<=mem->GetSize()))
    {
        tint rCount;
        const tubyte *x = reinterpret_cast<const tubyte *>(mem->GetData());
        
        rCount = static_cast<tint>((x[offset] >> 3) & 0x1f);
        if((x[offset] & 0x03)==2 && x[offset+1]==0xc9)
        {
            if((8 + (rCount * 24)) <= reportLen)
            {
                m_sessionID = NetMemory::toInt(mem,offset + 4);
                
                x = &x[offset + 8];
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
                
                done = reportLen;
            }
            else
            {
                printError("parse","Invalid RTCP reciever report as length as is less than expected");
            }
        }
        else
        {
            printError("parse","Invalid RTCP reciver report");
        }
    }
    else
    {
        printError("parse","Invalid RTCP reciever report as expected length is too short");
    }
    return done;
}

//-------------------------------------------------------------------------------------------

bool RTCPPacketRR::packet(NetArraySPtr mem)
{
    NetArray pMem;
    tint i,reportLen,bSize = (m_blocks.size() & 0x0000001f);
    tubyte *x;
    
    reportLen = 8 + (bSize * 24);
    pMem.SetSize(reportLen);
    x = reinterpret_cast<tubyte *>(pMem.GetData());
    
    x[0] = 0x02;
    x[0] |= (static_cast<tubyte>(bSize) << 3) & 0xf8;
    x[1] = 0xc9;
    NetMemory::fromShort(x,2,static_cast<tuint16>((reportLen / 4) - 1));
    NetMemory::fromInt(x,4,m_sessionID);
    
    x = &x[8];
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

tuint32 RTCPPacketRR::sessionID() const
{
    return m_sessionID;
}

//-------------------------------------------------------------------------------------------

void RTCPPacketRR::sessionID(tuint32 x)
{
    m_sessionID = x;
}

//-------------------------------------------------------------------------------------------

QList<RTCPReportBlock>& RTCPPacketRR::blockList()
{
    return m_blocks;
}

//-------------------------------------------------------------------------------------------

const QList<RTCPReportBlock>& RTCPPacketRR::blockList() const
{
    return m_blocks;
}

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
