#include "network/rtp/inc/RTCPPacketAPP.h"
#include "network/rtp/inc/Session.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

RTCPPacketAPP::RTCPPacketAPP(const Session& session) : RTCPPacket(session,RTCPPacket::e_Application),
    m_sessionID(0),
    m_name()
{}

//-------------------------------------------------------------------------------------------

RTCPPacketAPP::~RTCPPacketAPP()
{}

//-------------------------------------------------------------------------------------------

void RTCPPacketAPP::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "RTCPPacketAPP::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

tint RTCPPacketAPP::subType() const
{
    return m_subType;
}

//-------------------------------------------------------------------------------------------

void RTCPPacketAPP::subType(tint x)
{
    m_subType = x;
}

//-------------------------------------------------------------------------------------------

tuint32 RTCPPacketAPP::sessionID() const
{
    return m_sessionID;
}

//-------------------------------------------------------------------------------------------

void RTCPPacketAPP::sessionID(tuint32 x)
{
    m_sessionID = x;
}

//-------------------------------------------------------------------------------------------

const QString& RTCPPacketAPP::name() const
{
    return m_name;
}

//-------------------------------------------------------------------------------------------

void RTCPPacketAPP::name(const QString& x)
{
    m_name = x;
}

//-------------------------------------------------------------------------------------------

tint RTCPPacketAPP::parse(NetArraySPtr mem,tint offset)
{
    tint done = -1,reportLen = length(mem,offset);
    
    if(reportLen>=12 && (offset + reportLen)<=mem->GetSize())
    {
        const tubyte *x = reinterpret_cast<const tubyte *>(mem->GetData());
        tubyte t[5] = {0x00,0x00,0x00,0x00,0x00};
        
        x = &x[offset];
        if((x[0] & 0x03)==0x02 && x[1]==0xcc)
        {
            m_sessionID = NetMemory::toInt(x,4);
            ::memcpy(t,&x[8],4);
            m_name = QString::fromLatin1(reinterpret_cast<const tbyte *>(t));
            
            done = reportLen;
        }
        else
        {
            printError("parse","Invalid RTCP application packet");
        }
    }
    else
    {
        printError("parse","Invalid RTCP sender report as expected length is too short");
    }
    return done;
}

//-------------------------------------------------------------------------------------------

bool RTCPPacketAPP::packet(NetArraySPtr mem)
{
    tubyte t[12];
    QByteArray a;
    
    t[0] = 0x02;
    t[0] |= (static_cast<tubyte>(m_subType) << 3) & 0xf1;
    t[1] = 0xcc;
    NetMemory::fromShort(t,2,2);
    NetMemory::fromInt(t,4,m_sessionID);
    mem->AppendRaw(reinterpret_cast<const tbyte *>(t),8);
    a = m_name.toLatin1();
    if(a.length()>4)
    {
        a.chop(a.length() - 4);
    }
    ::memset(&t[8],0,4);
    ::memcpy(&t[8],a.constData(),a.length());
    
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
