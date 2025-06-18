//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_TCPSERVICE_H
#define __OMEGA_NETWORK_TCPSERVICE_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Service.h"
#include "network/inc/Controller.h"
#include "network/inc/TCPServerSocket.h"
#include "network/inc/TCPConnClientSocket.h"
#include "network/inc/TCPConnServerSocket.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

class NETWORK_EXPORT TCPService : public Service
{
    public:
        Q_OBJECT
        
    public:
        TCPService(QObject *parent = 0);
        virtual ~TCPService();
        
        virtual bool process();

        virtual void addConnection(TCPConnectionSocket *s);
        virtual void delConnection(TCPConnectionSocket *s);

        virtual bool start();
        virtual void stop();

    protected:
    
        QSet<TCPServerSocket *> m_serverSet;
        QSet<TCPConnectionSocket *> m_socketSet;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

