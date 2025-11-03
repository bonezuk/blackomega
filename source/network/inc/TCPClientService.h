//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_TCPCLIENTSERVICE_H
#define __OMEGA_NETWORK_TCPCLIENTSERVICE_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Service.h"
#include "network/inc/TCPConnClientSocket.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

class NETWORK_EXPORT TCPClientService : public Service
{
    public:
        Q_OBJECT

    public:
        TCPClientService(QObject *parent = 0);
        virtual ~TCPClientService();
        
        virtual bool start();
        virtual void stop();

        virtual void addConnection(TCPConnectionSocket *s);
        virtual void delConnection(TCPConnectionSocket *s);

        virtual bool process();
        
    protected:
        
        QSet<TCPConnectionSocket *> m_clientSet;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

