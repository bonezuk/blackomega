#include "network/rtp/inc/RTCPPacketBYE.h"
#include "network/rtp/inc/Session.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

RTCPPacketBYE::RTCPPacketBYE(const Session& session) : RTCPPacket(session,RTCPPacket::e_Goodbye),
    m_sessionList(),
    m_reason()
{}

//-------------------------------------------------------------------------------------------

RTCPPacketBYE::~RTCPPacketBYE()
{}

//-------------------------------------------------------------------------------------------

void RTCPPacketBYE::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "RTCPPacketBYE::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

const QString& RTCPPacketBYE::reason() const
{
    return m_reason;
}

//-------------------------------------------------------------------------------------------

void RTCPPacketBYE::reason(const QString& x)
{
    m_reason = x;
}

//-------------------------------------------------------------------------------------------

tuint32 RTCPPacketBYE::sessionID() const
{
    if(m_sessionList.size()>0)
    {
        return m_sessionList.first();
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------

QList<tuint32>& RTCPPacketBYE::sessionList()
{
    return m_sessionList;
}

//-------------------------------------------------------------------------------------------

const QList<tuint32>& RTCPPacketBYE::sessionList() const
{
    return m_sessionList;
}

//-------------------------------------------------------------------------------------------

tint RTCPPacketBYE::parse(NetArraySPtr mem,tint offset)
{
    tint i,done = -1,reportLen = length(mem,offset);

    if(reportLen>=4 && (offset + reportLen)<=mem->GetSize())
    {
        tint sCount;
        const tubyte *x = reinterpret_cast<const tubyte *>(mem->GetData());
        x = &x[offset];

        if((x[0] & 0x03)==0x02 && x[1]==0xcb)
        {
            tint len;

            sCount = static_cast<tint>((x[0] >> 3) & 0x1f);
            for(i=0;i<sCount;++i)
            {
                tuint32 id = NetMemory::toInt(x,4 + (i * 4));
                m_sessionList.append(id);
            }
            len = (sCount * 4) + 4;

            if(len < reportLen)
            {
                x = &x[(sCount * 4) + 4];
                m_reason = QString::fromUtf8(reinterpret_cast<const tbyte *>(&x[1]),static_cast<tint>(x[0]));
            }
            done = reportLen;
        }
        else
        {
            printError("parse","Invalid RTCP goodbye packet");
        }
    }
    else
    {
        printError("parse","Invalid RTCP goodbye as expected length is too short");
    }
    return done;
}

//-------------------------------------------------------------------------------------------

bool RTCPPacketBYE::packet(NetArraySPtr mem)
{
    tint i,len,sCount = m_sessionList.size() & 0x0000001f;
    NetArray pMem;
    tubyte *x,t[4] = {0x02,0xcb,0x00,0x00};

    t[0] |= (static_cast<tubyte>(sCount) << 3) & 0xf8;
    pMem.AppendRaw(reinterpret_cast<const tbyte *>(t),4);
    for(i=0;i<sCount;++i)
    {
        NetMemory::fromInt(t,0,m_sessionList.at(i));
        pMem.AppendRaw(reinterpret_cast<const tbyte *>(t),4);
    }
    if(!m_reason.isEmpty())
    {
        QByteArray a(m_reason.toUtf8());
        if(a.length()>=256)
        {
            a.chop(a.length() - 255);
        }
        t[0] = static_cast<tubyte>(a.length());
        pMem.AppendRaw(reinterpret_cast<const tbyte *>(t),1);
        pMem.AppendRaw(a.constData(),a.length());

        while(pMem.GetSize() % 4)
        {
            t[0] = 0x00;
            pMem.AppendRaw(reinterpret_cast<const tbyte *>(t),1);
        }
    }

    x = reinterpret_cast<tubyte *>(pMem.GetData());
    len = pMem.GetSize();
    len = (len / 4) - 1;
    NetMemory::fromShort(x,2,static_cast<tuint16>(len));
    mem->Append(pMem);

    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
