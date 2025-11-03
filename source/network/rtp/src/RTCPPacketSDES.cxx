#include "network/rtp/inc/RTCPPacketSDES.h"
#include "network/rtp/inc/Session.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------
// SDESBlock
//-------------------------------------------------------------------------------------------

SDESBlock::SDESBlock() : m_sessionID(0),
    m_canonical(),
    m_user(),
    m_email(),
    m_phone(),
    m_location(),
    m_tool()
{}

//-------------------------------------------------------------------------------------------

SDESBlock::SDESBlock(const SDESBlock& rhs) : m_sessionID(0),
    m_canonical(),
    m_user(),
    m_email(),
    m_phone(),
    m_location(),
    m_tool()
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

SDESBlock::~SDESBlock()
{}

//-------------------------------------------------------------------------------------------

const SDESBlock& SDESBlock::operator = (const SDESBlock& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void SDESBlock::copy(const SDESBlock& rhs)
{
    m_sessionID = rhs.m_sessionID;
    m_canonical = rhs.m_canonical;
    m_user = rhs.m_user;
    m_email = rhs.m_email;
    m_phone = rhs.m_phone;
    m_location = rhs.m_location;
    m_tool = rhs.m_tool;
    m_note = rhs.m_note;
    m_lastReceived = rhs.m_lastReceived;
}

//-------------------------------------------------------------------------------------------

tuint32 SDESBlock::sessionID() const
{
    return m_sessionID;
}

//-------------------------------------------------------------------------------------------

void SDESBlock::sessionID(tuint32 x)
{
    m_sessionID = x;
}

//-------------------------------------------------------------------------------------------

const QString& SDESBlock::canonical() const
{
    return m_canonical;
}

//-------------------------------------------------------------------------------------------

void SDESBlock::canonical(const QString& x)
{
    m_canonical = x;
}

//-------------------------------------------------------------------------------------------

const QString& SDESBlock::user() const
{
    return m_user;
}

//-------------------------------------------------------------------------------------------

void SDESBlock::user(const QString& x)
{
    m_user = x;
}

//-------------------------------------------------------------------------------------------

const QString& SDESBlock::email() const
{
    return m_email;
}

//-------------------------------------------------------------------------------------------

void SDESBlock::email(const QString& x)
{
    m_email = x;
}

//-------------------------------------------------------------------------------------------

const QString& SDESBlock::phone() const
{
    return m_phone;
}

//-------------------------------------------------------------------------------------------

void SDESBlock::phone(const QString& x)
{
    m_phone = x;
}

//-------------------------------------------------------------------------------------------

const QString& SDESBlock::location() const
{
    return m_location;
}

//-------------------------------------------------------------------------------------------

void SDESBlock::location(const QString& x)
{
    m_location = x;
}

//-------------------------------------------------------------------------------------------

const QString& SDESBlock::tool() const
{
    return m_tool;
}

//-------------------------------------------------------------------------------------------

void SDESBlock::tool(const QString& x)
{
    m_tool = x;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& SDESBlock::lastReceived() const
{
    return m_lastReceived;
}

//-------------------------------------------------------------------------------------------

void SDESBlock::lastReceived(const common::TimeStamp& x)
{
    m_lastReceived = x;
}

//-------------------------------------------------------------------------------------------

const QString& SDESBlock::note() const
{
    return m_note;
}

//-------------------------------------------------------------------------------------------

void SDESBlock::note(const QString& x)
{
    m_note = x;
}

//-------------------------------------------------------------------------------------------
// RTCPPacketSDES
//-------------------------------------------------------------------------------------------

RTCPPacketSDES::RTCPPacketSDES(const Session& session) : RTCPPacket(session,RTCPPacket::e_SourceDescription),
    m_blocks()
{}

//-------------------------------------------------------------------------------------------

RTCPPacketSDES::~RTCPPacketSDES()
{}

//-------------------------------------------------------------------------------------------

void RTCPPacketSDES::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "RTCPPacketSDES::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

tuint32 RTCPPacketSDES::sessionID() const
{
    if(m_blocks.size()>0)
    {
        return m_blocks.first().sessionID();
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------

QList<SDESBlock>& RTCPPacketSDES::blockList()
{
    return m_blocks;
}

//-------------------------------------------------------------------------------------------

const QList<SDESBlock>& RTCPPacketSDES::blockList() const
{
    return m_blocks;
}

//-------------------------------------------------------------------------------------------

tint RTCPPacketSDES::parse(NetArraySPtr mem,tint offset)
{
    tint done = -1, reportLen = length(mem,offset);
    
    if(reportLen>=4 && (offset + reportLen)<=mem->GetSize())
    {
        tint i,j,sCount;
        const tubyte *x = reinterpret_cast<const tubyte *>(mem->GetData());
        
        x = &x[offset];
        if((x[0] & 0x03)==0x02 && x[1]==0xca)
        {
            sCount = static_cast<tint>((x[0] >> 3) & 0x1f);
            for(i=0,j=4;i<sCount;++i)
            {
                SDESBlock block;
                bool loop = true;
                
                block.sessionID(NetMemory::toInt(x,j));
                j += 4;
                
                while(loop && ((j + 2) < reportLen))
                {
                    tint len = static_cast<tint>(x[j+1]);
                    
                    if((j+len+2) <= reportLen)
                    {
                        if(x[j]>=0x01 && x[j]<=0x07)
                        {
                            tint pType = static_cast<tint>(x[j]);
                            QString n = QString::fromUtf8(reinterpret_cast<const tbyte *>(&x[j+2]),len);
                            
                            switch(pType)
                            {
                                case 1:
                                    block.canonical(n);
                                    break;
                                    
                                case 2:
                                    block.user(n);
                                    break;
                                    
                                case 3:
                                    block.email(n);
                                    break;
                                    
                                case 4:
                                    block.phone(n);
                                    break;
                                    
                                case 5:
                                    block.location(n);
                                    break;
                                
                                case 6:
                                    block.tool(n);
                                    break;
                                
                                case 7:
                                    block.note(n);
                                    break;
                            }
                            j += len + 2;
                        }
                        else
                        {
                            if(x[j]==0x00)
                            {
                                j++;
                                while(j & 0x00000003)
                                {
                                    j++;
                                }
                                loop = false;
                            }
                            else
                            {
                                j += len + 2;
                            }
                        }
                    }
                }
                
                while(j & 0x00000003)
                {
                    j++;
                }
                
                m_blocks.append(block);
            }
            
            done = j;
        }
        else
        {
            printError("parse","Invalid RTCP source description");
        }
    }
    else
    {
        printError("parse","Invalid RTCP source description as expected length is too short");
    }
    return done;
}

//-------------------------------------------------------------------------------------------

bool RTCPPacketSDES::packet(NetArraySPtr mem)
{
    tint i,len,sCount = m_blocks.size() & 0x0000001f;
    NetArray pMem;
    tubyte *x,t[4] = {0x02,0xca,0x00,0x00};
    
    t[0] |= (static_cast<tubyte>(sCount) << 3) & 0xf1;
    pMem.AppendRaw(reinterpret_cast<const tbyte *>(t),4);
    for(i=0;i<sCount;++i)
    {
        QByteArray a;
        const SDESBlock& block = m_blocks.at(i);
        
        NetMemory::fromInt(t,0,block.sessionID());
        pMem.AppendRaw(reinterpret_cast<const tbyte *>(t),4);
        
        // canonical name
        t[0] = 0x01;
        a = block.canonical().toUtf8();
        if(a.length()>=256)
        {
            a.chop(a.length() - 255);
        }
        t[1] = static_cast<tubyte>(a.length());
        pMem.AppendRaw(reinterpret_cast<const tbyte *>(t),2);
        pMem.AppendRaw(a.constData(),a.length());
        
        // user = 0x02
        if(!block.user().isEmpty())
        {
            t[0] = 0x02;
            a = block.user().toUtf8();
            if(a.length()>=256)
            {
                a.chop(a.length() - 255);
            }
            t[1] = static_cast<tubyte>(a.length());
            pMem.AppendRaw(reinterpret_cast<const tbyte *>(t),2);
            pMem.AppendRaw(a.constData(),a.length());
        }
        
        // email = 0x03
        if(!block.email().isEmpty())
        {
            t[0] = 0x03;
            a = block.email().toUtf8();
            if(a.length()>=256)
            {
                a.chop(a.length() - 255);
            }
            t[1] = static_cast<tubyte>(a.length());
            pMem.AppendRaw(reinterpret_cast<const tbyte *>(t),2);
            pMem.AppendRaw(a.constData(),a.length());
        }
        
        // phone = 0x04
        if(!block.phone().isEmpty())
        {
            t[0] = 0x04;
            a = block.phone().toUtf8();
            if(a.length()>=256)
            {
                a.chop(a.length() - 255);
            }
            t[1] = static_cast<tubyte>(a.length());
            pMem.AppendRaw(reinterpret_cast<const tbyte *>(t),2);
            pMem.AppendRaw(a.constData(),a.length());
        }
        
        // location = 0x05
        if(!block.location().isEmpty())
        {
            t[0] = 0x05;
            a = block.location().toUtf8();
            if(a.length()>=256)
            {
                a.chop(a.length() - 255);
            }
            t[1] = static_cast<tubyte>(a.length());
            pMem.AppendRaw(reinterpret_cast<const tbyte *>(t),2);
            pMem.AppendRaw(a.constData(),a.length());
        }
        
        // tool = 0x06
        if(!block.tool().isEmpty())
        {
            t[0] = 0x06;
            a = block.tool().toUtf8();
            if(a.length()>=256)
            {
                a.chop(a.length() - 255);
            }
            t[1] = static_cast<tubyte>(a.length());
            pMem.AppendRaw(reinterpret_cast<const tbyte *>(t),2);
            pMem.AppendRaw(a.constData(),a.length());
        }
        
        // note = 0x07
        if(!block.note().isEmpty())
        {
            t[0] = 0x07;
            a = block.note().toUtf8();
            if(a.length()>=256)
            {
                a.chop(a.length() - 255);
            }
            t[1] = static_cast<tubyte>(a.length());
            pMem.AppendRaw(reinterpret_cast<const tbyte *>(t),2);
            pMem.AppendRaw(a.constData(),a.length());
        }

        ::memset(t,0,4 * sizeof(tubyte));
        len = pMem.GetSize() % 4;
        pMem.AppendRaw(reinterpret_cast<const tbyte *>(t),4 - len);
    }
    
    x = reinterpret_cast<tubyte *>(pMem.GetData());
    x[0] = (x[0] & 0x07) | ((static_cast<tubyte>(sCount) << 3) & 0xf8);
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
