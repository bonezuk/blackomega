//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_DATAPACKET_H
#define __OMEGA_NETWORK_RTP_DATAPACKET_H
//-------------------------------------------------------------------------------------------

#include "network/inc/UDPRead.h"
#include "common/inc/TimeStamp.h"
#include "network/rtp/inc/NetworkRTPDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

class NETWORKRTP_EXPORT DataPacket
{
    public:
        DataPacket();
        DataPacket(const DataPacket& rhs);

        const DataPacket& operator = (const DataPacket& rhs);

        tint type() const;
        void setType(tint x);

        const common::TimeStamp& time() const;
        void setTime(const common::TimeStamp& x);

        const common::TimeStamp& length() const;
        void setLength(const common::TimeStamp& x);

        NetArraySPtr data() const;
        void setData(NetArraySPtr x);

    protected:

        tint m_type;
        common::TimeStamp m_time;
        common::TimeStamp m_length;
        NetArraySPtr m_data;

        void copy(const DataPacket& rhs);
};

//-------------------------------------------------------------------------------------------

inline tint DataPacket::type() const
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

inline void DataPacket::setType(tint x)
{
    m_type = x;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& DataPacket::time() const
{
    return m_time;
}

//-------------------------------------------------------------------------------------------

inline void DataPacket::setTime(const common::TimeStamp& x)
{
    m_time = x;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& DataPacket::length() const
{
    return m_length;
}

//-------------------------------------------------------------------------------------------

inline void DataPacket::setLength(const common::TimeStamp& x)
{
    m_length = x;
}

//-------------------------------------------------------------------------------------------

class NETWORKRTP_EXPORT DataPacketList
{
    public:
        DataPacketList(QList<DataPacket>& list);

        QList<DataPacket>& list();
        const QList<DataPacket>& list() const;

    protected:

        QList<DataPacket> m_list;
};

//-------------------------------------------------------------------------------------------

inline QList<DataPacket>& DataPacketList::list()
{
    return m_list;
}

//-------------------------------------------------------------------------------------------

inline const QList<DataPacket>& DataPacketList::list() const
{
    return m_list;
}

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

