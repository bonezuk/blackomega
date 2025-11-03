#include "network/rtp/inc/SourceState.h"
#include "network/rtp/inc/Session.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------
// SourceStateDebugItem
//-------------------------------------------------------------------------------------------

SourceStateDebugItem::SourceStateDebugItem() : common::DebugOutputItem("SourceState",5),
    m_sourceID(0),
    m_sequenceNo(0),
    m_receive(),
    m_time()
{}

//-------------------------------------------------------------------------------------------

SourceStateDebugItem::SourceStateDebugItem(const common::DebugOutputItem& item) : common::DebugOutputItem("SourceState",5),
    m_sourceID(0),
    m_sequenceNo(0),
    m_receive(),
    m_time()
{
    copy(item);
}

//-------------------------------------------------------------------------------------------

tuint32& SourceStateDebugItem::sourceID()
{
    return m_sourceID;
}

//-------------------------------------------------------------------------------------------

tuint16& SourceStateDebugItem::sequenceNo()
{
    return m_sequenceNo;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp& SourceStateDebugItem::receive()
{
    return m_receive;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp& SourceStateDebugItem::time()
{
    return m_time;
}

//-------------------------------------------------------------------------------------------

void SourceStateDebugItem::copy(const common::DebugOutputItem& item)
{
    try
    {
        const SourceStateDebugItem& sItem = dynamic_cast<const SourceStateDebugItem&>(item);
        m_sourceID = sItem.m_sourceID;
        m_sequenceNo = sItem.m_sequenceNo;
        m_receive = sItem.m_receive;
        m_time = sItem.m_time;
    }
    catch(...) {}
    common::DebugOutputItem::copy(item);
}

//-------------------------------------------------------------------------------------------

QString SourceStateDebugItem::print() const
{
    QString x;
    
    x  = printStamp();
    x += QString::number(m_sourceID);
    x += " , ";
    x += QString::number(m_sequenceNo);
    x += " , ";
    x += printTime(m_time);
    x += " , ";
    x += printTime(m_receive);
    return x;
}

//-------------------------------------------------------------------------------------------
// SourceState
//-------------------------------------------------------------------------------------------

SourceState::SourceState(Session& session,tuint32 sourceID) : m_session(session),
    m_sourceID(sourceID),
    m_packetList(),
    m_maxSeq(0),
    m_cycles(0),
    m_baseSeq(0),
    m_badSeq(0),
    m_probation(c_seqMinSequential),
    m_received(0),
    m_expectedPrior(0),
    m_receivedPrior(0),
    m_transit(),
    m_jitter(0.0),
    m_lastSeqProcessed(0),
    m_lastSRTimeStamp(),
    m_lastSRRecieve(),
    m_startClockTime(),
    m_randomTimeOffset()
{}

//-------------------------------------------------------------------------------------------

SourceState::~SourceState()
{}

//-------------------------------------------------------------------------------------------

void SourceState::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "SourceState::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void SourceState::init(RTPPacketSPtr& p)
{
    tuint32 s = p->sequenceNo();
    
    m_sourceID = p->sourceID();
    m_baseSeq = s;
    m_maxSeq = s - 1;
    m_badSeq = RTP_SEQ_MOD + 1;
    m_cycles = 0;
    m_received = 0;
    m_receivedPrior = 0;
    m_expectedPrior = 0;
    m_probation = c_seqMinSequential;
    m_lastSRTimeStamp = 0;
    m_lastSRRecieve = 0;
    m_startClockTime = Session::clockTime();
    m_randomTimeOffset = p->time();
    m_transit = 0;
    m_jitter = 0.0;
    m_lastSeqProcessed = s - 1;
    m_packetList.clear();
}

//-------------------------------------------------------------------------------------------

SourceState::UpdateState SourceState::update(RTPPacketSPtr& p)
{
    tuint16 seq = static_cast<tuint16>(p->sequenceNo());
    tuint16 udelta = seq - m_maxSeq;
    
    common::TimeStamp arrival;
    common::TimeStamp pTime;
    common::TimeStamp transit;
    common::TimeStamp d;
    
    arrival  = Session::clockTime();
    arrival -= m_startClockTime;
    pTime  = p->time();
    pTime -= m_randomTimeOffset;
    if(arrival >= pTime)
    {
        transit = arrival - pTime;
    }
    else
    {
        transit = pTime - arrival;
    }
    if(transit >= m_transit)
    {
        d = transit - m_transit;
    }
    else
    {
        d = m_transit - transit;
    }
    m_transit = transit;
    m_jitter += (static_cast<tfloat64>(d) - static_cast<tfloat64>(m_jitter)) / 16.0;
    
#if defined(OMEGA_DEBUG_LOG)
    bool debugFlag = (common::DebugOutput::instance().level() >= 5) ? true : false;
    if(debugFlag)
    {
        common::DebugOutput& dOutput = common::DebugOutput::instance();
        SourceStateDebugItem item;
        
        item.sourceID() = p->sourceID();
        item.sequenceNo() = p->sequenceNo();
        item.receive() = arrival;
        item.time() = pTime;
        dOutput.append(item);
    }
#endif
        
    // Source is not valid until c_seqMinSequential packets with
    // sequential sequence numbers have been received,
    if(m_probation)
    {
        // packet is in sequence
        if(seq==m_maxSeq + 1)
        {
            m_probation--;
            m_maxSeq = seq;
            if(!m_probation)
            {
                initSequence(seq);
                m_received++;
                return e_Process;
            }
        }
        else
        {
            m_probation = c_seqMinSequential - 1;
            m_maxSeq = seq;
        }
        return e_Hold;
    }
    else if(udelta < c_seqMaxDropout)
    {
        // in order, with permissible gap
        if(seq < m_maxSeq)
        {
            // sequence number wrapped - count another 64K cycle.
            m_cycles += RTP_SEQ_MOD;
        }
        m_maxSeq = seq;
    }
    else if(udelta <= (RTP_SEQ_MOD - c_seqMaxMisorder))
    {
        // the sequence number made a very large jump
        if(seq==m_badSeq)
        {
            // Two sequential packets -- assume that the other side
            // restarted without telling us so just re-sync
            // (i.e. pretend this was the first packet).
            initSequence(seq);
        }
        else
        {
            m_badSeq = (seq + 1) & (RTP_SEQ_MOD - 1);
        }
        return e_Drop;
    }
    else
    {
        // duplicate or reordered packet
    }
    m_received++;
    return e_Process;
}

//-------------------------------------------------------------------------------------------

void SourceState::initSequence(tuint16 seq)
{
    m_baseSeq = seq;
    m_maxSeq = seq;
    m_badSeq = RTP_SEQ_MOD + 1;
    m_cycles = 0;
    m_received = 0;
    m_receivedPrior = 0;
    m_expectedPrior = 0;
}

//-------------------------------------------------------------------------------------------

void SourceState::onSR(const RTCPPacketSR& r)
{
    m_lastSRTimeStamp = r.time();
    m_lastSRRecieve = Session::clockTime();
}

//-------------------------------------------------------------------------------------------

bool SourceState::processPacket(RTPPacketSPtr& p,QList<DataPacket>& pList)
{
    tint pos = 0;
    bool res = false;
    
    pList.clear();
    
    if(p.data()==0)
    {
        printError("processPacket","No RTP packet given");
        return false;    
    }
    
    switch(update(p))
    {
        case e_Process:
            {
                p->setCycles(m_cycles);
                if(m_lastSeqProcessed < p->sequenceNo())
                {
                    if(!packetPosition(m_packetList,p,pos))
                    {
                        if(pos >= 0)
                        {
                            m_packetList.insert(pos,p);
                        }
                    }
                    
                    if(m_packetList.size() > 0)
                    {
                        tuint32 c,w;
                        bool loop = true;

                        while(loop)
                        {
                            c = m_packetList.at(0)->sequenceNo();

                            while(loop && m_lastSeqProcessed==(c-1))
                            {
                                RTPPacketSPtr pPacket = m_packetList.takeFirst();
                                    
                                if(m_session.unpackPacket(pPacket,pList))
                                {
                                    res = true;
                                }
                                m_lastSeqProcessed = c;

                                if(m_packetList.size()>0)
                                {
                                    c = m_packetList.at(0)->sequenceNo();
                                }
                                else
                                {
                                    loop = false;
                                }
                            }

                            w = m_packetList.size();
                            if(w >= c_packetProcessThreshold)
                            {
                                m_lastSeqProcessed = m_packetList.at(0)->sequenceNo() - 1;
                            }
                            else
                            {
                                loop = false;
                            }
                        }
                    }
                }
            }
            break;
            
        case e_Drop:
            p->setCycles(m_cycles);
            m_packetList.clear();
            m_packetList.insert(0,p);
            m_lastSeqProcessed = p->sequenceNo() - 1;
            break;
            
        case e_Hold:
            p->setCycles(m_cycles);
            if(m_lastSeqProcessed < p->sequenceNo())
            {
                if(!packetPosition(m_packetList,p,pos))
                {
                    if(pos>=0)
                    {
                        m_packetList.insert(pos,p);
                    }
                }
            }
            break;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// find the position of packet 'p' in 'list' calculating its actual position if it already
// exists in the list or the insert position if it doesn't already exist. returns:
//    true   - if the packet already exists in the list.
//    false  - if the packet doesn't exist.
//-------------------------------------------------------------------------------------------

bool SourceState::packetPosition(const QList<RTPPacketSPtr>& list,RTPPacketSPtr& p,tint& pos)
{
    tint size = list.size();
    bool res = false;
    
    if(size > 0)
    {
        tuint32 fSeq,lSeq,cSeq,gSeq;
        
        cSeq = p->sequenceNo();
        fSeq = list.first()->sequenceNo();
        lSeq = list.last()->sequenceNo();
        
        if(cSeq < fSeq)
        {
            pos = -1;
        }
        else if(lSeq < cSeq)
        {
            pos = size;
        }
        else
        {
            tint fPos,lPos,gPos;
            
            fPos = 0;
            lPos = size - 1;
            
            while(!res)
            {
                gPos = fPos + ((lPos - fPos) / 2);
                gSeq = list.at(gPos)->sequenceNo();
                
                if(cSeq == gSeq)
                {
                    pos = gPos;
                    res = true;
                }
                else if(fPos == lPos)
                {
                    if(cSeq < gSeq)
                    {
                        pos = (fPos) ? fPos - 1 : 0;
                    }
                    else
                    {
                        pos = fPos + 1;
                    }
                    break;
                }
                else if(cSeq < gSeq)
                {
                    lPos = gPos;
                }
                else
                {
                    fPos = gPos + 1;
                }
            }
        }
    }
    else
    {
        pos = 0;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void SourceState::report(RTCPReportBlock& block)
{
    tint32 extendedMax,expected,lost;
    tint32 expectedInterval,receivedInterval;
    tint32 lostInterval,fraction;
    common::TimeStamp delaySR;
    
    extendedMax = static_cast<tint32>(m_cycles) + static_cast<tint32>(m_maxSeq);
    expected = extendedMax - static_cast<tint32>(m_baseSeq) + 1;
    lost = expected - static_cast<tint32>(m_received);
    
    expectedInterval = expected - static_cast<tint32>(m_expectedPrior);
    m_expectedPrior = expected;
    
    receivedInterval = static_cast<tint32>(m_received) - static_cast<tint32>(m_receivedPrior);
    m_receivedPrior = m_received;
    
    lostInterval = expectedInterval - receivedInterval;
    if(expectedInterval==0 || lostInterval<=0)
    {
        fraction = 0;
    }
    else
    {
        fraction = (lostInterval << 8) / expectedInterval;
    }
    
    delaySR = Session::clockTime() - m_lastSRRecieve;
    
    block.sessionID(m_sourceID);
    block.fractionLost(fraction);
    block.packetsLost(lost);
    block.highestSequenceNo(static_cast<tuint32>(extendedMax));
    block.interarrivalJitter(static_cast<tuint32>(m_jitter));
    block.lastSR(m_lastSRTimeStamp);
    block.delayLastSR(delaySR);
}

//-------------------------------------------------------------------------------------------

void SourceState::resync()
{
    tuint32 s = m_maxSeq;
    
    m_baseSeq = s;
    m_maxSeq = s;
    m_lastSeqProcessed = s;
    m_badSeq = RTP_SEQ_MOD + 1;
    m_cycles = 0;
    m_received = 0;
    m_receivedPrior = 0;
    m_expectedPrior = 0;
    m_probation = c_seqMinSequential;
    m_transit = 0;
    m_jitter = 0.0;
    m_packetList.clear();
}

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
