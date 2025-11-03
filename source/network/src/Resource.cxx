#include "network/inc/Resource.h"
#include "network/inc/NetMemory.h"
#include "common/inc/Log.h"
#include "common/inc/Exception.h"
#include "common/inc/BString.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

Resource *Resource::m_instance = 0;

//-------------------------------------------------------------------------------------------

Resource::Resource() : m_hostMap(),
    m_ipMap()
{
#if defined(OMEGA_WIN32)
    WSADATA data;

    if(::WSAStartup(MAKEWORD(1,1),&data))
    {
        throw common::Exception("Failed to start TCP/IP services");
    }
#endif
    buildLocalIP();
}

//-------------------------------------------------------------------------------------------

Resource::~Resource()
{
#if defined(OMEGA_WIN32)
    ::WSACleanup();
#endif
}

//-------------------------------------------------------------------------------------------

void Resource::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "Resource::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

Resource& Resource::instance()
{
    if(m_instance==0)
    {
        m_instance = new Resource;
    }
    return *m_instance;
}

//-------------------------------------------------------------------------------------------

QString Resource::username() const
{
    QString name;

#if defined(OMEGA_WIN32)
    tbyte tmp[256];
    DWORD size = 256;

    if(::GetUserNameA(tmp,&size)!=0)
    {
        name = QString::fromUtf8(tmp);
    }
    else
    {
        if(::GetLastError()==ERROR_INSUFFICIENT_BUFFER)
        {
            tbyte *tmpB = new tbyte [size];
            if(::GetUserNameA(tmpB,&size))
            {
                name = QString::fromUtf8(tmpB);
            }
            delete tmpB;
        }
    }
#elif defined(OMEGA_POSIX)
    tchar *n = ::getlogin();

    if(n==0)
    {
        struct passwd *pwdEntry;

        pwdEntry = ::getpwuid(::getuid());
        if(pwdEntry!=0)
        {
            name = QString::fromUtf8(pwdEntry->pw_name);
        }
        else
        {
            n = ::getenv("LOGNAME");
            if(n!=0)
            {
                name = QString::fromUtf8(n);
            }
        }
    }
    else
    {
        name = QString::fromUtf8(n);
    }
#endif

    return name;
}

//-------------------------------------------------------------------------------------------

QString Resource::hostname() const
{
    struct hostent *host;
    QString name;
    tchar tmp[512];

    if(::gethostname(tmp,512)==0)
    {
        host = ::gethostbyname(tmp);
        if(host!=0)
        {
            name = QString::fromLatin1(::inet_ntoa(*(reinterpret_cast<struct in_addr *>(host->h_addr))));
        }
        else
        {
            name = QString::fromLatin1("127.0.0.1");
        }
    }
    else
    {
        name = QString::fromLatin1("127.0.0.1");
    }
    name = name.toLower();
    return name;
}

//-------------------------------------------------------------------------------------------

QString Resource::webname() const
{
    struct hostent *host;
    QString name;
    tchar tmp[512];

    if(::gethostname(tmp,512)==0)
    {
        host = ::gethostbyname(tmp);

        if(host!=0)
        {
            name = QString::fromUtf8(host->h_name);
        }
        else
        {
            name = QString::fromLatin1("localhost");
        }
    }
    else
    {
        name = QString::fromLatin1("localhost");
    }
    name = name.toLower();
    return name;
}

//-------------------------------------------------------------------------------------------

void Resource::getNames(QStringList& nameList) const
{
    struct hostent *host;
    QString name;
    tchar tmp[512];

    nameList.clear();

    if(::gethostname(tmp,512)==0)
    {
        name = QString::fromUtf8(tmp);
        nameList.append(name);

        host = ::gethostbyname(tmp);
        if(host!=0)
        {
            getIPList(host,nameList);
        }
    }
    else
    {
        host = ::gethostbyname("localhost");
        if(host!=0)
        {
            getIPList(host,nameList);
        }
        else
        {
            name = QString::fromLatin1("localhost");
            nameList.append(name);
        }
    }
}

//-------------------------------------------------------------------------------------------

void Resource::getIPList(struct hostent *host,QStringList& nameList) const
{
    if(host!=0)
    {
        int i;
        QString name;
        QStringList::iterator ppI;

        for(i=0;host->h_addr_list[i]!=0;++i)
        {
            name = QString::fromLatin1(::inet_ntoa(*(reinterpret_cast<struct in_addr *>(host->h_addr_list[i]))));
            for(ppI=nameList.begin();ppI!=nameList.end();++ppI)
            {
                if(name==*ppI)
                {
                    break;
                }
            }
            if(ppI==nameList.end())
            {
                nameList.append(name);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

tuint32 Resource::getIP(const struct sockaddr_in *addr) const
{
    tuint32 ip;

#if defined(OMEGA_WIN32)
    ip = addr->sin_addr.S_un.S_addr;
#elif defined(OMEGA_POSIX)
    ip = addr->sin_addr.s_addr;
#endif
    ip = ntohl(ip);
    return ip;
}

//-------------------------------------------------------------------------------------------

bool Resource::isLocal(const QString& str)
{
    bool res = false;

    if(str.isEmpty())
    {
        QString name(str.toLower());

        if(name=="localhost" || name=="127.0.0.1" || name==webname() || name==hostname())
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool Resource::isLocal(const struct sockaddr_in *addr)
{
    tuint32 ip;
    QSet<tuint32>::const_iterator ppI;

    if(m_localMap.isEmpty())
    {
        buildLocalIP();
    }

#if defined(OMEGA_WIN32)
    ip = addr->sin_addr.S_un.S_addr;
#elif defined(OMEGA_POSIX)
    ip = addr->sin_addr.s_addr;
#endif
    ip = ntohl(ip);
    if(ip==0x7f000001)
    {
        return true;
    }
    else
    {
        ppI = m_localMap.find(ip);
        return (ppI!=m_localMap.end()) ? true : false;
    }
}

//-------------------------------------------------------------------------------------------

tuint32 Resource::localIP()
{
    tuint32 lIP = ntohl(static_cast<tuint32>(::inet_addr("127.0.0.1")));
    QSet<tuint32>::const_iterator ppI;

    if(m_localMap.isEmpty())
    {
        buildLocalIP();
    }
    for(ppI=m_localMap.begin();ppI!=m_localMap.end();ppI++)
    {
        tuint32 ip = *ppI;

        if(ip!=lIP)
        {
            return ip;
        }
    }
    return lIP;
}

//-------------------------------------------------------------------------------------------
// Find local IP interface address into which an incoming connection is from
//-------------------------------------------------------------------------------------------

tuint32 Resource::localIP(const struct sockaddr_in *addr)
{
    tuint32 cIP, dIP;
    tuint32 lIP = ntohl(static_cast<tuint32>(::inet_addr("127.0.0.1")));
    QSet<tuint32>::const_iterator ppI;

#if defined(OMEGA_WIN32)
    cIP = addr->sin_addr.S_un.S_addr;
#elif defined(OMEGA_POSIX)
    cIP = addr->sin_addr.s_addr;
#endif
    cIP = ntohl(cIP);

    if(m_localMap.isEmpty())
    {
        buildLocalIP();
    }
    dIP = lIP;
    for(ppI=m_localMap.begin();ppI!=m_localMap.end();ppI++)
    {
        tuint32 ip = *ppI;

        if(ip!=lIP)
        {
            tuint32 cDiff = (cIP > dIP) ? cIP - dIP : dIP - cIP;
            tuint32 nDiff = (ip > cIP) ? ip - cIP : cIP - ip;
            if(nDiff < cDiff)
            {
                dIP = ip;
            }
        }
    }
    return dIP;
}

//-------------------------------------------------------------------------------------------

QString Resource::localIPName()
{
    QString name;
    tuint32 lIP = localIP();

    name  = QString::number(lIP & 0x000000ff) + ".";
    name += QString::number((lIP >>  8) & 0x000000ff) + ".";
    name += QString::number((lIP >> 16) & 0x000000ff) + ".";
    name += QString::number((lIP >> 24) & 0x000000ff);
    return name;
}

//-------------------------------------------------------------------------------------------

QString Resource::ip(const QString& name) const
{
    struct hostent *host;
    QString nIP;

    if(!name.isEmpty())
    {
        host = ::gethostbyname(name.toLatin1().constData());
        if(host!=0)
        {
            nIP = QString::fromLatin1(::inet_ntoa(*(reinterpret_cast<struct in_addr *>(host->h_addr))));
        }
    }
    return nIP;
}

//-------------------------------------------------------------------------------------------

bool Resource::isIPAddress(const QString& name) const
{
    tint i=0,j=0,num[4]={0,0,0,0};
    bool res = false;

    if(!name.isEmpty())
    {
        while(i < name.size())
        {
            QChar c = name.at(i);

            if(c==QChar('.'))
            {
                j++;
                if(j>3)
                {
                    return false;
                }
            }
            else if(c.isDigit())
            {
                num[j] *= 10;
                num[j] += static_cast<tint>(c.toLatin1() - '0');
            }
            else
            {
                return false;
            }
            ++i;
        }
        if(j<3)
        {
            return false;
        }
        for(i=0;i<4;++i)
        {
            if(num[i] > 255)
            {
                return false;
            }
        }
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void Resource::setAddressIP(tuint32 ip,struct sockaddr_in *addr) const
{
#if defined(OMEGA_WIN32)
    addr->sin_addr.S_un.S_addr = ip;
#elif defined(OMEGA_POSIX)
    addr->sin_addr.s_addr = ip;
#endif
}

//-------------------------------------------------------------------------------------------

void Resource::getAddress(const QString& host,tint port,struct sockaddr_in *addr)
{
    QMap<QString,tuint32>::iterator ppI;
    tuint32 ip;

    addr->sin_family = AF_INET;

    if(!host.isEmpty())
    {
        ppI = m_hostMap.find(host);

        if(ppI==m_hostMap.end())
        {
            if(!isIPAddress(host))
            {
                struct hostent *h;

                h = ::gethostbyname(host.toLatin1().constData());
                if(h!=0)
                {
                    ::memcpy(&ip,h->h_addr,sizeof(tuint32));
                    setAddressIP(ip,addr);
                    m_hostMap.insert(host,ip);
                }
                else
                {
                    setAddressIP(INADDR_NONE,addr);
                }
            }
            else
            {
                ip = ::inet_addr(host.toLatin1().constData());
                setAddressIP(ip,addr);
                m_hostMap.insert(host,ip);
            }
        }
        else
        {
            setAddressIP(ppI.value(),addr);
        }
    }
    else
    {
        setAddressIP(INADDR_NONE,addr);
    }

#if defined(OMEGA_WIN32)
    addr->sin_port = ::htons(static_cast<u_short>(port));
#elif defined(OMEGA_POSIX)
    addr->sin_port = htons(port);
#endif
}

//-------------------------------------------------------------------------------------------

bool Resource::findAddress(QString& host,tint& port,const struct sockaddr_in *addr,bool dnsLookup)
{
    QMap<tuint32,QString>::iterator ppI;
    tuint32 ip;
    bool res = true;

#if defined(OMEGA_WIN32)
    ip = addr->sin_addr.S_un.S_addr;
#elif defined(OMEGA_POSIX)
    ip = addr->sin_addr.s_addr;
#endif

    ppI = m_ipMap.find(ip);
    if(ppI==m_ipMap.end())
    {
        if(dnsLookup)
        {
            struct hostent *h;
#if defined(OMEGA_WIN32)
            h = ::gethostbyaddr(reinterpret_cast<const char *>(&ip),sizeof(tuint32),AF_INET);
#elif defined(OMEGA_POSIX)
            h = ::gethostbyaddr(reinterpret_cast<const void *>(&ip),sizeof(tuint32),AF_INET);
#endif
            if(h!=0)
            {
                host = QString::fromUtf8(h->h_name);
            }
            else
            {
                res = false;
            }
        }
        else
        {
            host = QString::fromUtf8(::inet_ntoa(addr->sin_addr));
        }

        if(!host.isEmpty())
        {
            m_ipMap.insert(ip,host);
        }
    }
    else
    {
        host = ppI.value();
    }

    if(host.isEmpty())
    {
        host = QString::fromUtf8(::inet_ntoa(addr->sin_addr));
    }

    port = ntohs(addr->sin_port);

    return res;
}

//-------------------------------------------------------------------------------------------

bool Resource::isMulticastIP(const QString& ipAddr) const
{
    return isMulticastIP(static_cast<tuint32>(htonl(::inet_addr(ipAddr.toLatin1().constData()))));
}

//-------------------------------------------------------------------------------------------

bool Resource::isMulticastIP(tuint32 ipAddr) const
{
    ipAddr = (ipAddr >> 24) & 0x000000ff;
    return (ipAddr>=224 && ipAddr<=239) ? true : false;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

void Resource::buildLocalIPFromInterfaces()
{
    tuint32 ip;
    int res;
    struct ifaddrs *interfaces = 0, *iface;
    QSet<tuint32>::iterator ppI;

    res = getifaddrs(&interfaces);
    if(!res)
    {
        iface = interfaces;
        while(iface != 0)
        {
            struct sockaddr_in *addr = (struct sockaddr_in *)(iface->ifa_addr);

            if(addr->sin_family==AF_INET)
            {
                ip = addr->sin_addr.s_addr;
                ip = ntohl(ip);
                ppI = m_localMap.find(ip);
                if(ppI == m_localMap.end())
                {
                    m_localMap.insert(ip);
                }
            }
            iface = iface->ifa_next;
        }
    }
    if(interfaces != 0)
    {
        freeifaddrs(interfaces);
    }
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

void Resource::buildLocalIPFromInterfaces()
{
    int iter;
    ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;
    PIP_ADAPTER_ADDRESSES pAddresses = NULL, pCurr = NULL;
    ULONG outBufLen = 15000; // Allocate a 15 KB buffer to start with.
    DWORD dwRetVal = 0;

    iter = 0;
    do
    {
        pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
        if(pAddresses != 0)
        {
            dwRetVal = GetAdaptersAddresses(AF_INET, flags, NULL, pAddresses, &outBufLen);
            if(dwRetVal == ERROR_BUFFER_OVERFLOW)
            {
                free(pAddresses);
                pAddresses = NULL;
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
        iter++;
    } while(dwRetVal == ERROR_BUFFER_OVERFLOW && iter < 3);

    if(pAddresses != 0)
    {
        if(dwRetVal == NO_ERROR)
        {
            pCurr = pAddresses;
            while(pCurr != NULL)
            {
                PIP_ADAPTER_UNICAST_ADDRESS_LH pUnicast = pCurr->FirstUnicastAddress;
                while(pUnicast != NULL)
                {
                    SOCKADDR *addr = pUnicast->Address.lpSockaddr;
                    if(addr != NULL)
                    {
                        if(addr->sa_family == AF_INET)
                        {
                            QSet<tuint32>::iterator ppI;
                            struct sockaddr_in *saddr = reinterpret_cast<struct sockaddr_in *>(addr);
                            tuint32 ip = saddr->sin_addr.S_un.S_addr;
                            ip = ntohl(ip);
                            ppI = m_localMap.find(ip);
                            if(ppI != m_localMap.end())
                            {
                                m_localMap.insert(ip);
                            }
                        }
                    }
                    pUnicast = pUnicast->Next;
                }
                pCurr = pCurr->Next;
            }
        }
        free(pAddresses);
    }
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

void Resource::buildLocalIPFromHostname()
{
    int i;
    tuint32 ip;
    hostent *h;
    char tmp[256];
    QSet<tuint32>::iterator ppI;

    m_localMap.clear();

    if(::gethostname(tmp,256)==0)
    {
        h = ::gethostbyname(tmp);
    }
    else
    {
        h = ::gethostbyname("localhost");
    }

    if(h!=0)
    {
        for(i=0;h->h_addr_list[i]!=0;++i)
        {
            struct in_addr *addr = reinterpret_cast<struct in_addr *>(h->h_addr_list[i]);

#if defined(OMEGA_WIN32)
            ip = addr->S_un.S_addr;
#elif defined(OMEGA_POSIX)
            ip = addr->s_addr;
#endif
            ip = ntohl(ip);
            if(ppI == m_localMap.end())
            {
                m_localMap.insert(ip);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void Resource::buildLocalIP()
{
    const tuint32 c_localIP = 0x7F000001; // 127.0.0.1
    QSet<tuint32>::iterator ppI;

    m_localMap.clear();
    buildLocalIPFromInterfaces();
    if(m_localMap.isEmpty())
    {
        buildLocalIPFromHostname();
    }
    ppI = m_localMap.find(c_localIP);
    if(ppI == m_localMap.end())
    {
        m_localMap.insert(c_localIP);
    }
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

void Resource::error(const tchar *strO,const tchar *strR,const tchar *strE,tint err) const
{
    LPVOID msgBuf = 0;
    DWORD res;
    common::BString str(strE);

    res = ::FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        0,
        static_cast<DWORD>(err),
        MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&msgBuf),
        0,
        0);

    str += ". ";
    if(res && msgBuf!=0)
    {
        str += reinterpret_cast<const tchar *>(msgBuf);
    }
    else
    {
        str += "System error number " + common::BString::Int(err);
    }

    common::Log::g_Log << strO << "::" << strR << " - " << str << common::c_endl;

    if(msgBuf!=0)
    {
        ::LocalFree(msgBuf);
    }
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

void Resource::error(const tchar *strO,const tchar *strR,const tchar *strE,tint err) const
{
    common::BString str(strE);

    str += ". ";
    str += ::strerror(err);
    common::Log::g_Log << strO << "::" << strR << " - " << str << common::c_endl;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

tuint32 Resource::random(tint type) const
{
    struct {
        tint type;
        time_t t;
        tint pid;
    } rs;

    struct tm *gmt;
    tchar name[512];
    DWORD len=512;
    common::SHA1Digest digest;
    tbyte rmem[common::c_SHA1HashSize];
    tuint32 res;

    rs.type = type;
    rs.t = ::clock();
    rs.pid = ::_getpid();
    gmt=::gmtime(&rs.t);
    ::memset(name,0,512 * sizeof(tchar));
    ::GetUserNameA(name,&len);

    digest.input(reinterpret_cast<const tbyte *>(&rs),sizeof(rs));
    digest.input(reinterpret_cast<const tbyte *>(gmt),sizeof(struct tm));
    digest.input(reinterpret_cast<const tbyte *>(name),static_cast<tint>(len) * sizeof(tchar));
    digest.getDigestFinal(rmem,common::c_SHA1HashSize * sizeof(tchar));
    res = NetMemory::toInt(rmem);
    return res;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

tuint32 Resource::random(tint type) const
{
    struct {
        tint type;
        struct timeval tv;
        clock_t cpu;
        pid_t pid;
        u_long hid;
        uid_t uid;
        gid_t gid;
        struct utsname name;
    } s;

    ::gettimeofday(&s.tv,0);
    ::uname(&s.name);
    s.type = type;
    s.cpu = ::clock();
    s.pid = ::getpid();
#ifndef OMEGA_ANDROID
    s.hid = ::gethostid();
#else
    s.hid = 0;
#endif
    s.uid = ::getuid();
    s.gid = ::getgid();

    common::SHA1Digest digest;
    tbyte rmem[common::c_SHA1HashSize];
    tuint32 res;

    digest.input(reinterpret_cast<const tbyte *>(rmem),sizeof(s));
    digest.getDigestFinal(rmem,common::c_SHA1HashSize * sizeof(tchar));
    res = NetMemory::toInt(rmem);
    return res;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

QString Resource::networkIPAddressToString(struct in_addr ipAddress) const
{
    char buf[256];
    const char *ipBuffer;
    QString ipText;

#if defined(OMEGA_WIN32)
    ipBuffer = inet_ntop(AF_INET,reinterpret_cast<PVOID>(&ipAddress),buf,256);
#elif defined(OMEGA_POSIX)
    ipBuffer = inet_ntop(AF_INET,reinterpret_cast<const void *>(&ipAddress),buf,256);
#endif
    if(ipBuffer!=0)
    {
        ipText = QString::fromLatin1(ipBuffer);
    }
    return ipText;
}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
