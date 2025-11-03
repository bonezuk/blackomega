//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_NTPCLIENT_H
#define __OMEGA_COMMON_NTPCLIENT_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"
#include "common/inc/TimeStamp.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

class NETWORK_EXPORT NTPClient
{
    public:
        NTPClient();
        virtual ~NTPClient();

        common::TimeStamp getTime(const QString& host);

        static common::TimeStamp toTimeStamp(const tchar *mem);
        static void fromTimeStamp(const common::TimeStamp& t,tchar *mem);

    protected:

        bool isReadable(socket_type s,const common::TimeStamp& t);
};

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
