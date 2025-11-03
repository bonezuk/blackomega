#include "network/inc/NTPClient.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

NTPClient::NTPClient()
{}

//-------------------------------------------------------------------------------------------

NTPClient::~NTPClient()
{}

//-------------------------------------------------------------------------------------------

common::TimeStamp NTPClient::toTimeStamp(const tchar *mem)
{
    tuint32 aL,aH;
    tuint64 a;
    common::TimeStamp t;

    aL =  (static_cast<tuint32>(mem[3]) & 0x000000ff) |
         ((static_cast<tuint32>(mem[2]) <<  8) & 0x0000ff00) |
         ((static_cast<tuint32>(mem[1]) << 16) & 0x00ff0000) |
         ((static_cast<tuint32>(mem[0]) << 24) & 0xff000000);
    aH =  (static_cast<tuint32>(mem[7]) & 0x000000ff) |
         ((static_cast<tuint32>(mem[6]) <<  8) & 0x0000ff00) |
         ((static_cast<tuint32>(mem[5]) << 16) & 0x00ff0000) |
         ((static_cast<tuint32>(mem[4]) << 24) & 0xff000000);
    a = ((static_cast<tuint64>(aL) << 32) & 0xffffffff00000000ULL) | (static_cast<tuint64>(aH) & 0x00000000ffffffffULL);
    t = a;
    return t;
}

//-------------------------------------------------------------------------------------------

void NTPClient::fromTimeStamp(const common::TimeStamp& t,tchar *mem)
{
    tuint32 aL,aH;
    tuint64 a;

    a = static_cast<tuint64>(t);
    aL = static_cast<tuint32>((a >> 32) & 0x00000000ffffffffULL);
    aH = static_cast<tuint32>(a & 0x00000000ffffffffULL);

    mem[0] = static_cast<tchar>((aL >> 24) & 0x000000ff);
    mem[1] = static_cast<tchar>((aL >> 16) & 0x000000ff);
    mem[2] = static_cast<tchar>((aL >>  8) & 0x000000ff);
    mem[3] = static_cast<tchar>(aL & 0x000000ff);
    mem[4] = static_cast<tchar>((aH >> 24) & 0x000000ff);
    mem[5] = static_cast<tchar>((aH >> 16) & 0x000000ff);
    mem[6] = static_cast<tchar>((aH >>  8) & 0x000000ff);
    mem[7] = static_cast<tchar>(aH & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

bool NTPClient::isReadable(socket_type s,const common::TimeStamp& t)
{
    fd_set readS;
    struct timeval tv;

    FD_ZERO(&readS);
    FD_SET(s,&readS);
    tv.tv_sec = t.secondsTotal();
    tv.tv_usec = t.millisecond();

    if(::select(s+1,&readS,0,0,&tv) > 0)
    {
        return (FD_ISSET(s,&readS)) ? true : false;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

common::TimeStamp NTPClient::getTime(const QString& host)
{
    socket_type s;
    common::TimeStamp actualTime;

    s = ::socket(AF_INET,SOCK_DGRAM,0);
    if(s!=c_invalidSocket)
    {
        struct sockaddr_in clientAddr,remoteAddr,serverAddr;

        ::memset(&clientAddr,0,sizeof(clientAddr));
        clientAddr.sin_family = AF_INET;
        clientAddr.sin_port = htons(0);
#if defined(OMEGA_WIN32)
        clientAddr.sin_addr.S_un.S_addr = ::htonl(INADDR_ANY);
#elif defined(OMEGA_POSIX)
        clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
        ::memcpy(&serverAddr,&clientAddr,sizeof(clientAddr));

        Resource::instance().getAddress(host,123,&remoteAddr);

        if(::bind(s,reinterpret_cast<struct sockaddr *>(&clientAddr),sizeof(clientAddr))==0)
        {
            tubyte nData[48];
            common::TimeStamp clientTime = common::TimeStamp::now();

            ::memset(nData,0,48);
            nData[0] = 0x1B;
            fromTimeStamp(clientTime,reinterpret_cast<tchar *>(&nData[40]));

            if(::sendto(s,reinterpret_cast<tchar *>(nData),48,0,reinterpret_cast<struct sockaddr *>(&remoteAddr),sizeof(remoteAddr))>0)
            {
                common::TimeStamp timeout(2.0);

                if(isReadable(s,timeout))
                {
                    tint dLen;

#if defined(OMEGA_WIN32)
                    tint serverAddrSize = sizeof(serverAddr);
                    dLen = ::recvfrom(s,reinterpret_cast<tchar *>(nData),48,0,reinterpret_cast<struct sockaddr *>(&remoteAddr),&serverAddrSize);
#elif defined(OMEGA_POSIX)
                    socklen_t serverAddrSize = sizeof(serverAddr);
                    dLen = ::recvfrom(s,reinterpret_cast<tchar *>(nData),48,0,reinterpret_cast<struct sockaddr *>(&remoteAddr),&serverAddrSize);
#endif

                    if(dLen>=48)
                    {
                        tfloat64 t1,t2,t3,t4,dT,tT;
                        common::TimeStamp destinationTime = common::TimeStamp::now();
                        common::TimeStamp receiveTime = toTimeStamp(reinterpret_cast<tchar *>(&nData[32]));
                        common::TimeStamp transmitTime = toTimeStamp(reinterpret_cast<tchar *>(&nData[40]));

                        t1 = static_cast<tfloat64>(clientTime);
                        t2 = static_cast<tfloat64>(receiveTime);
                        t3 = static_cast<tfloat64>(transmitTime);
                        t4 = static_cast<tfloat64>(destinationTime);

                        dT = (t4 - t1) - (t2 - t3);
                        tT = ((t2 - t1) + (t3 - t4)) / 2;

                        if(tT>=-10.0 && tT<=10.0)
                        {
                            actualTime = destinationTime + tT;
                        }
                        else
                        {
                            actualTime = transmitTime;
                        }
                    }
                }
            }
        }

#if defined(OMEGA_WIN32)
        ::closesocket(s);
#elif defined(OMEGA_POSIX)
        ::close(s);
#endif
    }
    return actualTime;
}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
