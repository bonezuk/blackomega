#include "network/rtp/inc/RTCPReportBlock.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------
// RTCPReportBlock
//-------------------------------------------------------------------------------------------

RTCPReportBlock::RTCPReportBlock() : m_sessionID(0),
    m_fractionLost(0),
    m_packetLost(0),
    m_highestSequenceNo(0),
    m_interarrivalJitter(0),
    m_lastSR(),
    m_delayLastSR()
{}

//-------------------------------------------------------------------------------------------

RTCPReportBlock::RTCPReportBlock(const RTCPReportBlock& rhs) : m_sessionID(0),
    m_fractionLost(0),
    m_packetLost(0),
    m_highestSequenceNo(0),
    m_interarrivalJitter(0),
    m_lastSR(),
    m_delayLastSR()
{
    copy(rhs);    
}

//-------------------------------------------------------------------------------------------

RTCPReportBlock::~RTCPReportBlock()
{}

//-------------------------------------------------------------------------------------------

const RTCPReportBlock& RTCPReportBlock::operator = (const RTCPReportBlock& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void RTCPReportBlock::copy(const RTCPReportBlock& rhs)
{
    m_sessionID = rhs.m_sessionID;
    m_fractionLost = rhs.m_fractionLost;
    m_packetLost = rhs.m_packetLost;
    m_highestSequenceNo = rhs.m_highestSequenceNo;
    m_interarrivalJitter = rhs.m_interarrivalJitter;
    m_lastSR = rhs.m_lastSR;
    m_delayLastSR = rhs.m_delayLastSR;
}

//-------------------------------------------------------------------------------------------

tuint32 RTCPReportBlock::sessionID() const
{
    return m_sessionID;
}

//-------------------------------------------------------------------------------------------

void RTCPReportBlock::sessionID(tuint32 x)
{
    m_sessionID = x;
}

//-------------------------------------------------------------------------------------------

tint RTCPReportBlock::fractionLost() const
{
    return m_fractionLost;
}

//-------------------------------------------------------------------------------------------

void RTCPReportBlock::fractionLost(tint x)
{
    m_fractionLost = x;
}

//-------------------------------------------------------------------------------------------

tint RTCPReportBlock::packetsLost() const
{
    return m_packetLost;
}

//-------------------------------------------------------------------------------------------

void RTCPReportBlock::packetsLost(tint x)
{
    m_packetLost = x;
}

//-------------------------------------------------------------------------------------------

tuint32 RTCPReportBlock::highestSequenceNo() const
{
    return m_highestSequenceNo;
}

//-------------------------------------------------------------------------------------------

void RTCPReportBlock::highestSequenceNo(tuint32 x)
{
    m_highestSequenceNo = x;
}

//-------------------------------------------------------------------------------------------

tuint32 RTCPReportBlock::interarrivalJitter() const
{
    return m_interarrivalJitter;
}

//-------------------------------------------------------------------------------------------

void RTCPReportBlock::interarrivalJitter(tuint32 x)
{
    m_interarrivalJitter = x;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& RTCPReportBlock::lastSR() const
{
    return m_lastSR;
}

//-------------------------------------------------------------------------------------------

void RTCPReportBlock::lastSR(const common::TimeStamp& x)
{
    m_lastSR = x;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& RTCPReportBlock::delayLastSR() const
{
    return m_delayLastSR;
}

//-------------------------------------------------------------------------------------------

void RTCPReportBlock::delayLastSR(const common::TimeStamp& x)
{
    m_delayLastSR = x;
}

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
