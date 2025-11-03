#include "network/rtp/inc/RTPPacket.h"
#include "network/rtp/inc/Session.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

RTPPacket::RTPPacket(Session& session,QObject *parent) : QObject(parent),
    m_session(session),
    m_headerFlag(false),
    m_sequenceNo(0xffffffff),
    m_header(),
    m_body(new NetArray())
{}

//-------------------------------------------------------------------------------------------

RTPPacket::~RTPPacket()
{}

//-------------------------------------------------------------------------------------------

void RTPPacket::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "RTPPacket::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

tint RTPPacket::version() const
{
    return 2;
}

//-------------------------------------------------------------------------------------------

bool RTPPacket::padding() const
{
    if(m_headerFlag)
    {
        const tubyte *x = reinterpret_cast<const tubyte *>(m_header.GetData());
        return (x[0] & 0x04) ? true : false;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

bool RTPPacket::extension() const
{
    if(m_headerFlag)
    {
        const tubyte *x = reinterpret_cast<const tubyte *>(m_header.GetData());
        return (x[0] & 0x08) ? true : false;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

bool RTPPacket::marker() const
{
    if(m_headerFlag)
    {
        const tubyte *x = reinterpret_cast<const tubyte *>(m_header.GetData());
        return (x[1] & 0x01) ? true : false;
    }
    else
    {
        return m_session.marker();
    }
}

//-------------------------------------------------------------------------------------------

tint RTPPacket::payloadType() const
{
    if(m_headerFlag)
    {
        const tubyte *x = reinterpret_cast<const tubyte *>(m_header.GetData());
        tint p = static_cast<tint>(static_cast<tuint>(x[1] & 0xfe) >> 1);
        return p;
    }
    else
    {
        return m_session.payloadType();
    }
}

//-------------------------------------------------------------------------------------------

tuint32 RTPPacket::sequenceNo() const
{
    if(m_headerFlag)
    {
        if(m_sequenceNo!=0xffffffff)
        {
            return m_sequenceNo;
        }
        else
        {
            const tubyte *x = reinterpret_cast<const tubyte *>(m_header.GetData());
            tuint32 p;
        
            p  =  static_cast<tuint32>(x[2]) & 0x000000ff;
            p |= (static_cast<tuint32>(x[3]) << 8) & 0x0000ff00;
            return static_cast<tint32>(p);
        }
    }
    else
    {
        return m_session.sequenceNo();
    }
}

//-------------------------------------------------------------------------------------------

void RTPPacket::setCycles(tuint32 x)
{
    if(m_sequenceNo==0xffffffff)
    {
        m_sequenceNo = (x & 0xffff0000) | (sequenceNo() & 0x0000ffff);
    }
}

//-------------------------------------------------------------------------------------------

common::TimeStamp RTPPacket::time() const
{
    if(m_headerFlag)
    {
        const tubyte *x = reinterpret_cast<const tubyte *>(m_header.GetData());
        common::TimeStamp t;
        tuint32 p;
        
        p  =  static_cast<tuint32>(x[4]) & 0x000000ff;
        p |= (static_cast<tuint32>(x[5]) <<  8) & 0x0000ff00;
        p |= (static_cast<tuint32>(x[6]) << 16) & 0x00ff0000;
        p |= (static_cast<tuint32>(x[7]) << 24) & 0xff000000;
        t = p;
        return t;
    }
    else
    {
        return m_session.time();
    }
}

//-------------------------------------------------------------------------------------------

tuint32 RTPPacket::sourceID() const
{
    if(m_headerFlag)
    {
        const tubyte *x = reinterpret_cast<const tubyte *>(m_header.GetData());
        tuint32 p;

        p  =  static_cast<tuint32>(x[ 8]) & 0x000000ff;
        p |= (static_cast<tuint32>(x[ 9]) <<  8) & 0x0000ff00;
        p |= (static_cast<tuint32>(x[10]) << 16) & 0x00ff0000;
        p |= (static_cast<tuint32>(x[11]) << 24) & 0xff000000;
        return p;        
    }
    else
    {
        return m_session.sourceID();
    }
}

//-------------------------------------------------------------------------------------------

tuint32 RTPPacket::contributingID(tint idx) const
{
    if(m_headerFlag)
    {
        if(idx < contributingNumber())
        {
            tint32 p;
            const tubyte *x = reinterpret_cast<const tubyte *>(m_header.GetData());
            tint offset = 12 + (idx * 4);
            
            p  =  static_cast<tuint32>(x[0 + offset]) & 0x000000ff;
            p |= (static_cast<tuint32>(x[1 + offset]) <<  8) & 0x0000ff00;
            p |= (static_cast<tuint32>(x[2 + offset]) << 16) & 0x00ff0000;
            p |= (static_cast<tuint32>(x[3 + offset]) << 24) & 0xff000000;
            return p;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return m_session.contributingID(idx);
    }
}

//-------------------------------------------------------------------------------------------

tint RTPPacket::contributingNumber() const
{
    if(m_headerFlag)
    {
        const tubyte *x = reinterpret_cast<const tubyte *>(m_header.GetData());
        tuint p;
        
        p = static_cast<tuint>(x[0] & 0xf0) >> 4;
        return static_cast<tint>(p);
    }
    else
    {
        return m_session.contributingNumber();
    }
}

//-------------------------------------------------------------------------------------------

void RTPPacket::buildHeader()
{
    tint i,j,size = 12 + (m_session.contributingNumber() * 4);
    tuint p;
    tubyte *x;
    
    m_headerFlag = false;
    m_header.SetSize(size);
    x = reinterpret_cast<tubyte *>(m_header.GetData());
    
    x[0]  = 0x02;
    x[0] |= padding() ? 0x04 : 0x00;
    x[0] |= extension() ? 0x08 : 0x00;
    x[0] |= (static_cast<tubyte>(m_session.contributingNumber()) << 4) & 0xf0;
    
    x[1]  = marker() ? 0x01 : 0x00;
    x[1] |= (static_cast<tubyte>(m_session.payloadType()) << 1) & 0xfe;
    
    p = static_cast<tuint>(m_session.sequenceNo()) & 0x0000ffff;
    x[2] = static_cast<tubyte>(p & 0x000000ff);
    x[3] = static_cast<tubyte>((p >> 8) & 0x000000ff);
    
    p = static_cast<common::TimeStamp>(m_session.rtime());
    x[4] = static_cast<tubyte>(p & 0x000000ff);
    x[5] = static_cast<tubyte>((p >>  8) & 0x000000ff);
    x[6] = static_cast<tubyte>((p >> 16) & 0x000000ff);
    x[7] = static_cast<tubyte>((p >> 24) & 0x000000ff);
    
    p = m_session.sourceID();
    x[ 8] = static_cast<tubyte>(p & 0x000000ff);
    x[ 9] = static_cast<tubyte>((p >>  8) & 0x000000ff);
    x[10] = static_cast<tubyte>((p >> 16) & 0x000000ff);
    x[11] = static_cast<tubyte>((p >> 24) & 0x000000ff);
    
    for(i=0,j=12;i<m_session.contributingNumber();++i,j+=4)
    {
        p = m_session.contributingID(j);
        x[j+0] = static_cast<tubyte>(p & 0x000000ff);
        x[j+1] = static_cast<tubyte>((p >>  8) & 0x000000ff);
        x[j+2] = static_cast<tubyte>((p >> 16) & 0x000000ff);
        x[j+3] = static_cast<tubyte>((p >> 24) & 0x000000ff);        
    }

    m_headerFlag = true;
}

//-------------------------------------------------------------------------------------------

NetArraySPtr RTPPacket::body()
{
    return m_body;
}

//-------------------------------------------------------------------------------------------

void RTPPacket::setBody(NetArraySPtr bPtr)
{
    m_body = bPtr;
}

//-------------------------------------------------------------------------------------------

bool RTPPacket::packet(NetArraySPtr mem)
{
    bool res = true;
    
    if(!m_headerFlag)
    {
        buildHeader();
    }
    
    if(m_body.data()!=0)
    {
        mem->SetSize(0);
        mem->Append(m_header);
        mem->Append(*(m_body.data()));
    }
    else
    {
        printError("packet","Packet contains no body data");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint RTPPacket::parse(NetArraySPtr mem,tint offset)
{
    tint done = -1;
    
    if((offset + 12) <= mem->GetSize())
    {
        const tubyte *x = reinterpret_cast<const tubyte *>(mem->GetData());
        
        x = &x[offset];
        m_header.SetSize(0);
        m_header.AppendRaw(reinterpret_cast<const tbyte *>(x),12);
        m_headerFlag = true;
        
        if((x[0] & 0x03)==0x02)
        {
            if(m_session.isSupportedPayload(payloadType()))
            {
                tint ccNo = contributingNumber();
                
                if(ccNo>0)
                {
                    if((offset + 12 + (ccNo * 4)) <= static_cast<tint>(mem->GetSize()))
                    {
                        m_header.AppendRaw(reinterpret_cast<const tbyte *>(&x[12]),ccNo * 4);
                        offset += ccNo * 4;
                    }
                    else
                    {
                        printError("parse","Invalid RTP packet, header shorter than expected");
                        return -1;
                    }
                }
                offset += 12;
                
                tint len = m_session.getRTPBody(*this,mem,offset,m_body);
                if(len>=0)
                {
                    done = offset + len;
                }
                else
                {
                    printError("parse","Invalid RTP packet, error parsing body of RTP packet");
                }
            }
            else
            {
                printError("parse","Invalid RTP packet, session doesn't support given payload type");
            }
        }
        else
        {
            printError("parse","Invalid RTP packet, invalid version number");
        }
    }
    else
    {
        printError("parse","Invalid RTP packet, no header");
    }
    return done;
}

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
