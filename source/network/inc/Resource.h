//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RESOURCE_H
#define __OMEGA_NETWORK_RESOURCE_H
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------
#include <WS2spi.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

#include "network/inc/NetworkDLL.h"

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

#include <process.h>

namespace omega
{
namespace network
{

typedef SOCKET socket_type;
typedef int addrlen_type;
const socket_type c_invalidSocket = INVALID_SOCKET;

} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pwd.h>
#include <ifaddrs.h>

namespace omega
{
namespace network
{

typedef int socket_type;
typedef socklen_t addrlen_type;
const socket_type c_invalidSocket = -1;

} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>
#include <QVector>
#include <QSet>
#include <QPair>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <QSharedPointer>

#include "common/inc/Array.h"
#include "common/inc/SHA1Digest.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------


typedef common::Array<tubyte,tubyte> NetArray;
typedef NetArray* NetArrayPtr;
typedef QSharedPointer<NetArray> NetArraySPtr;

//-------------------------------------------------------------------------------------------

class NETWORK_EXPORT Resource
{
    public:
        ~Resource();

        static Resource& instance();

        QString username() const;
        QString hostname() const;
        QString webname() const;

        void getNames(QStringList& nameList) const;

        bool isLocal(const QString& str);
        bool isLocal(const struct sockaddr_in *addr);

        tuint32 getIP(const struct sockaddr_in *addr) const;
        tuint32 localIP();
        tuint32 localIP(const struct sockaddr_in *addr);
        QString localIPName();

        QString ip(const QString& name) const;

        bool isIPAddress(const QString& name) const;

        void setAddressIP(tuint32 ip,struct sockaddr_in *addr) const;

        void getAddress(const QString& host,tint port,struct sockaddr_in *addr);
        bool findAddress(QString& host,tint& port,const struct sockaddr_in *addr,bool dnsLookup = true);

        void error(const tchar *strO,const tchar *strR,const tchar *strE,tint err) const;

        bool isMulticastIP(const QString& ipAddr) const;
        bool isMulticastIP(tuint32 ipAddr) const;

        tuint32 random(tint type = 0) const;

        QString networkIPAddressToString(struct in_addr ipAddress) const;

    protected:

        static Resource *m_instance;

        QMap<QString,tuint32> m_hostMap; // dns name   -> ip address
        QMap<tuint32,QString> m_ipMap;   // ip address -> dns name
        QSet<tuint32> m_localMap;

        Resource();

        void printError(const tchar *strR,const tchar *strE) const;

        void getIPList(struct hostent *host,QStringList& nameList) const;

        void buildLocalIPFromInterfaces();
        void buildLocalIPFromHostname();
        void buildLocalIP();
};

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

