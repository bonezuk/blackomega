#include "network/rtp/inc/RTCPPacket.h"
#include "network/rtp/inc/Session.h"
#include "network/rtp/inc/RTCPPacketSR.h"
#include "network/rtp/inc/RTCPPacketRR.h"
#include "network/rtp/inc/RTCPPacketAPP.h"
#include "network/rtp/inc/RTCPPacketBYE.h"
#include "network/rtp/inc/RTCPPacketSDES.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

RTCPPacket::RTCPPacket(const Session& session,Type t) : m_session(session),
    m_type(t)
{}

//-------------------------------------------------------------------------------------------

RTCPPacket::~RTCPPacket()
{}

//-------------------------------------------------------------------------------------------

void RTCPPacket::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "RTCPPacket::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

RTCPPacket::Type RTCPPacket::type() const
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

bool RTCPPacket::isPacket(NetArraySPtr mem,tint offset)
{
    return (getPacketType(mem,offset)!=e_Unknown) ? true : false;
}

//-------------------------------------------------------------------------------------------

RTCPPacket::Type RTCPPacket::getPacketType(NetArraySPtr mem,tint offset)
{
    Type t = e_Unknown;
    tint len = mem->GetSize();
    const tubyte *x = reinterpret_cast<const tubyte *>(mem->GetData());

    if(x!=0 && (offset + 2)<=len)
    {
        tuint pt = static_cast<tuint>(x[offset + 1]);

        if(pt==200) // sender report
        {
            t = e_SenderReport;
        }
        else if(pt==201) // reciever report
        {
            t = e_RecieverReport;
        }
        else if(pt==202) // source description
        {
            t = e_SourceDescription;
        }
        else if(pt==203) // goodbye
        {
            t = e_Goodbye;
        }
        else if(pt==204) // application
        {
            t = e_Unknown;
        }
    }
    return t;
}

//-------------------------------------------------------------------------------------------

tint RTCPPacket::generate(const Session& session,QVector<RTCPPacketSPtr>& pVector,NetArraySPtr mem,tint offset)
{
    tint nOffset,len = mem->GetSize();

    pVector.clear();

    while(offset < len)
    {
        RTCPPacketSPtr p;
        Type t = getPacketType(mem,offset);

        switch(t)
        {
            case e_SenderReport:
                {
                    RTCPPacketSPtr nP(reinterpret_cast<RTCPPacket *>(new RTCPPacketSR(session)));
                    p = nP;
                }
                break;

            case e_RecieverReport:
                {
                    RTCPPacketSPtr nP(reinterpret_cast<RTCPPacket *>(new RTCPPacketRR(session)));
                    p = nP;
                }
                break;

            case e_SourceDescription:
                {
                    RTCPPacketSPtr nP(reinterpret_cast<RTCPPacket *>(new RTCPPacketSDES(session)));
                    p = nP;
                }
                break;

            case e_Goodbye:
                {
                    RTCPPacketSPtr nP(reinterpret_cast<RTCPPacket *>(new RTCPPacketBYE(session)));
                    p = nP;
                }
                break;

            case e_Application:
                {
                    RTCPPacketSPtr nP(reinterpret_cast<RTCPPacket *>(new RTCPPacketAPP(session)));
                    p = nP;
                }
                break;

            case e_Unknown:
                {
                    RTCPPacketSPtr nP;
                    p = nP;
                }
                break;
        }

        if(p.data()!=0)
        {
            tint len = length(mem,offset);

            if(len>=0 && (offset + len)<=mem->GetSize())
            {
                nOffset = p->parse(mem,offset);
                if(nOffset>0)
                {
                    pVector.append(p);
                    offset += nOffset;
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    return offset;
}

//-------------------------------------------------------------------------------------------

void RTCPPacket::append(RTCPPacketSPtr p,NetArraySPtr mem)
{
    if(p.data()!=0 && mem.data()!=0)
    {
        NetArraySPtr m(new NetArray);

        if(p->packet(m))
        {
            mem->Append(*(m.data()));
        }
    }
}

//-------------------------------------------------------------------------------------------

tint RTCPPacket::length(NetArraySPtr mem,tint offset)
{
    tint l = -1,len = mem->GetSize();
    const tubyte *x = reinterpret_cast<const tubyte *>(mem->GetData());

    if(x!=0 && (offset + 4) <= len)
    {
        tuint i;

        i  =  static_cast<tuint>(x[offset + 2]) & 0x000000ff;
        i |= (static_cast<tuint>(x[offset + 3]) << 8) & 0x0000ff00;
        i++;
        l = static_cast<tint>(i << 2);
    }
    return l;
}

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
