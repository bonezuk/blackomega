#include "network/rtp/inc/DataPacket.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

DataPacket::DataPacket() : m_type(0),
    m_time(),
    m_length(),
    m_data()
{}

//-------------------------------------------------------------------------------------------

DataPacket::DataPacket(const DataPacket& rhs) : m_type(0),
    m_time(),
    m_length(),
    m_data()
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

const DataPacket& DataPacket::operator = (const DataPacket& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void DataPacket::copy(const DataPacket& rhs)
{
    m_type = rhs.m_type;
    m_time = rhs.m_time;
    m_length = rhs.m_length;
    m_data = rhs.m_data;
}

//-------------------------------------------------------------------------------------------

NetArraySPtr DataPacket::data() const
{
    return m_data;
}

//-------------------------------------------------------------------------------------------

void DataPacket::setData(NetArraySPtr x)
{
    m_data = x;
}

//-------------------------------------------------------------------------------------------
// DataPacketList
//-------------------------------------------------------------------------------------------

DataPacketList::DataPacketList(QList<DataPacket>& list) : m_list(list)
{}

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
