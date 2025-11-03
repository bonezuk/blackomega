#include "network/rtp/silveromega/inc/SilverSession.h"

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

CONCRETE_QOBJECT_FACTORY_CLASS_IMPL(SessionFactory,Session, \
                                    SilverSessionFactory,SilverSession, \
                                    "ogg",false)

//-------------------------------------------------------------------------------------------

const tint c_maxMTUPacketSize = 1024;

//-------------------------------------------------------------------------------------------

SilverSession::SilverSession(QObject *parent) : Session(parent),
    m_packets(),
    m_tCalc(0)
{}

//-------------------------------------------------------------------------------------------

SilverSession::~SilverSession()
{
    try
    {
        stop();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void SilverSession::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "SilverSession::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool SilverSession::openResource(const QString& resource)
{
    QString err;
    bool res = true;

    if(m_packets.open(resource))
    {
        res = true;
    }
    else
    {
        err = "Failed to open vorbis file '" + resource + "' for RTP session.";
        printError("openResource",err.toLatin1().constData());
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void SilverSession::closeResource()
{
    m_packets.close();
}

//-------------------------------------------------------------------------------------------

bool SilverSession::generatePacketData(common::TimeStamp& t,RTPPacketSPtr p)
{
    return buildPacket(t,*(p->body().data()));
}

//-------------------------------------------------------------------------------------------

bool SilverSession::buildPacket(common::TimeStamp& ts,NetArray& mem)
{
    tubyte h[6],*p;
    tint frag,amount,packetNo = 0;
    tuint id = m_packets.serialID();

    mem.SetSize(0);

    h[0] = static_cast<tubyte>(id & 0x000000ff);
    h[1] = static_cast<tubyte>((id >>  8) & 0x000000ff);
    h[2] = static_cast<tubyte>((id >> 16) & 0x000000ff);

    if(m_lastPacket.GetSize()>0 && m_lastPacketOffset>0)
    {
        // continuation of fragmented packet
        p = m_lastPacket.GetData();

        amount = m_lastPacket.GetSize() - m_lastPacketOffset;
        if(amount > c_maxMTUPacketSize)
        {
            amount = c_maxMTUPacketSize;
            frag = 2;
        }
        else
        {
            frag = 3;
        }

        h[3] = static_cast<tubyte>((static_cast<tuint>(frag)) & 0x00000003);
        h[4] = static_cast<tubyte>(static_cast<tuint>(amount) & 0x000000ff);
        h[5] = static_cast<tubyte>((static_cast<tuint>(amount) >> 8) & 0x000000ff);

        mem.AppendRaw(reinterpret_cast<const tbyte *>(h),6);
        mem.AppendRaw(reinterpret_cast<const tbyte *>(&p[m_lastPacketOffset]),amount);
        ts = m_lastPacketTime;

        if(frag==2)
        {
            m_lastPacketOffset += amount;
        }
        else
        {
            m_lastPacketOffset = 0;
            m_lastPacket.SetSize(0);
        }
    }
    else
    {
        if(m_lastPacket.GetSize()==0)
        {
            if(!m_packets.next(m_lastPacket,m_lastPacketTime))
            {
                return false;
            }
            m_lastPacketOffset = 0;
        }

        if(m_lastPacket.GetSize() > c_maxMTUPacketSize)
        {
            p = m_lastPacket.GetData();

            h[3] = 0x01;
            amount = c_maxMTUPacketSize;
            h[4] = static_cast<tubyte>(static_cast<tuint>(amount) & 0x000000ff);
            h[5] = static_cast<tubyte>((static_cast<tuint>(amount) >> 8) & 0x000000ff);
            mem.AppendRaw(reinterpret_cast<const tbyte *>(h),6);
            mem.AppendRaw(reinterpret_cast<const tbyte *>(p),c_maxMTUPacketSize);
            m_lastPacketOffset = c_maxMTUPacketSize;
            ts = m_lastPacketTime;
        }
        else
        {
            tint offset = 0;

            h[3] = 0;
            mem.AppendRaw(reinterpret_cast<const tbyte *>(h),4);
            ts = m_lastPacketTime;

            while((offset + m_lastPacket.GetSize())<=c_maxMTUPacketSize && packetNo<15)
            {
                amount = m_lastPacket.GetSize();
                h[0] = static_cast<tubyte>(static_cast<tuint>(amount) & 0x000000ff);
                h[1] = static_cast<tubyte>((static_cast<tuint>(amount) >> 8) & 0x000000ff);
                mem.AppendRaw(reinterpret_cast<const tbyte *>(h),2);
                mem.Append(m_lastPacket);
                packetNo++;
                offset += m_lastPacket.GetSize() + 2;
                if(!m_packets.next(m_lastPacket,m_lastPacketTime))
                {
                    break;
                }
            }
            p = mem.GetData();
            p[3] |= (static_cast<tubyte>(packetNo) << 4) & 0xf0;
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

tint SilverSession::getRTPBody(const RTPPacket& p,NetArraySPtr mem,tint offset,NetArraySPtr body)
{
    tint len = 0,mLen = mem->GetSize() - offset;
    const tubyte *x = reinterpret_cast<const tubyte *>(mem->GetData());
    const tubyte *y = reinterpret_cast<const tubyte *>(mem->GetData());

    x += offset;
    if(mLen>=6)
    {
        tint frag = static_cast<tint>(x[3] & 0x03);
        tint pkts = static_cast<tint>((x[3] >> 4) & 0x0f);
        tint amount;

        if(frag)
        {
            amount  =  static_cast<tint>(x[4]) & 0x000000ff;
            amount |= (static_cast<tint>(x[5]) << 8) & 0x0000ff00;
            len = amount + 6;
        }
        else
        {
            tint i = 0;

            mLen -= 4;
            len = 4;
            x += 4;

            while(mLen>=2 && i<pkts)
            {
                amount  =  static_cast<tint>(x[0]) & 0x000000ff;
                amount |= (static_cast<tint>(x[1]) << 8) & 0x0000ff00;
                amount += 2;
                if(amount<=mLen)
                {
                    mLen -= amount;
                    len += amount;
                    x += amount;
                }
                else
                {
                    break;
                }
                i++;
            }
        }

        body->SetSize(0);
        body->AppendRaw(reinterpret_cast<const tbyte *>(&y[offset]),len);
    }
    return len;
}

//-------------------------------------------------------------------------------------------

bool SilverSession::unpackPacket(RTPPacketSPtr& p,QList<DataPacket>& pList)
{
    tint frag,type,pkts,amount;
    NetArraySPtr mem(p->body());
    bool res = false;
    common::TimeStamp ts(p->time()),pLen;
    SourceStateSPtr source;
    QMap<tuint32,SourceStateSPtr>::iterator ppI;

    ppI = m_sourceSessionMap.find(p->sourceID());
    if(ppI!=m_sourceSessionMap.end())
    {
        source = ppI.value();
        ts -= source->randomClock();
    }

    if(mem.data()!=0 && mem->GetSize()>6)
    {
        const tubyte *x = reinterpret_cast<const tubyte *>(mem->GetData());

        frag = static_cast<tint>(x[3] & 0x03);
        type = static_cast<tint>((x[3] >> 2) & 0x03);
        pkts = static_cast<tint>((x[3] >> 4) & 0x0f);

        switch(frag)
        {
            case 0:
                {
                    tint amount,len = mem->GetSize() - 4;

                    x += 4;
                    while(len>2 && pkts>0)
                    {
                        amount  =  static_cast<tint>(x[0]) & 0x000000ff;
                        amount |= (static_cast<tint>(x[1]) << 8) & 0x0000ff00;
                        if(amount>0 && (amount + 2)<=len)
                        {
                            DataPacket pData;
                            NetArraySPtr m(new NetArray);

                            m->AppendRaw(reinterpret_cast<const tbyte *>(&x[2]),amount);
                            if(m_tCalc!=0)
                            {
                                pLen = static_cast<tfloat64>(m_tCalc->calcLength(*(m.data()))) / static_cast<tfloat64>(m_tCalc->frequency());
                            }
                            else
                            {
                                pLen = 0;
                            }

                            pData.setType(type);
                            pData.setData(m);
                            pData.setTime(ts);
                            pData.setLength(pLen);
                            pList.append(pData);
                            ts += pLen;


                            x += amount + 2;
                            len -= amount + 2;
                            res = true;
                        }
                        else
                        {
                            x += 2;
                            len -= 2;
                        }
                        pkts--;
                    }
                }
                break;

            case 1:
                {
                    amount  =  static_cast<tint>(x[4]) & 0x000000ff;
                    amount |= (static_cast<tint>(x[5]) << 8) & 0x0000ff00;
                    m_lastPacket.SetSize(0);
                    if(amount <= (mem->GetSize() - 6))
                    {
                        m_lastPacket.AppendRaw(reinterpret_cast<const tbyte *>(&x[6]),amount);
                    }
                }
                break;

            case 2:
                {
                    amount  =  static_cast<tint>(x[4]) & 0x000000ff;
                    amount |= (static_cast<tint>(x[5]) << 8) & 0x0000ff00;
                    if(amount <= (mem->GetSize() - 6))
                    {
                        m_lastPacket.AppendRaw(reinterpret_cast<const tbyte *>(&x[6]),amount);
                    }
                }
                break;

            case 3:
                {
                    DataPacket pData;
                    NetArraySPtr m(new NetArray);

                    amount  =  static_cast<tint>(x[4]) & 0x000000ff;
                    amount |= (static_cast<tint>(x[5]) << 8) & 0x0000ff00;
                    m_lastPacket.SetSize(0);
                    if(amount <= (mem->GetSize() - 6))
                    {
                        m_lastPacket.AppendRaw(reinterpret_cast<const tbyte *>(&x[6]),amount);
                    }

                    m->Append(m_lastPacket);
                    m_lastPacket.SetSize(0);
                    if(m_tCalc!=0)
                    {
                        pLen = static_cast<tfloat64>(m_tCalc->calcLength(*(m.data()))) / static_cast<tfloat64>(m_tCalc->frequency());
                    }
                    else
                    {
                        pLen = 0;
                    }

                    pData.setType(type);
                    pData.setData(m);
                    pData.setTime(ts);
                    pData.setLength(pLen);
                    pList.append(pData);
                    if(m_tCalc!=0)
                    {
                        ts += static_cast<tfloat64>(m_tCalc->calcLength(*(m.data()))) / static_cast<tfloat64>(m_tCalc->frequency());
                    }
                    res = true;
                }
                break;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void SilverSession::setTCalc(VTimeCalc *calc)
{
    m_tCalc = calc;
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
