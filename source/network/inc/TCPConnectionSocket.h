//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_TCPCONNECTIONSOCKET_H
#define __OMEGA_NETWORK_TCPCONNECTIONSOCKET_H
//-------------------------------------------------------------------------------------------

#include "network/inc/TCPServerSocket.h"
#include "network/inc/MemoryPacket.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

class NETWORK_EXPORT TCPConnectionSocket : public Socket
{
    public:
        Q_OBJECT

    public:
        TCPConnectionSocket(Service *svr,QObject *parent = 0);
        virtual ~TCPConnectionSocket();

        virtual void close();

        virtual bool canRead() const;
        virtual bool canWrite() const;

        virtual bool doRead();
        virtual bool doWrite();

        virtual bool write(const tbyte *mem,tint len);
        virtual bool write(const tubyte *mem,tint len);

        virtual tint read(tbyte *mem,tint len);
        virtual tint read(tubyte *mem,tint len);

        virtual bool canRead(tint len);
        virtual bool canGetNextLine();
        virtual bool getNextLine(QString& line);
        virtual bool getNextLine(common::BString& line);
        virtual tint available();

        virtual void setForRead();
        virtual void setForWrite();

        virtual bool isTimer() const;
        virtual void timerOn();
        virtual void timerOff();
        virtual bool timeout();
        virtual bool timeout(time_t t);

        virtual tint writeQueueSize();

        virtual bool process();

        virtual bool isError() const;

        virtual void lock();
        virtual void unlock();
        virtual bool isLocked();

    protected:

        static QList<MemoryPacket *> m_freePackets;

        TCPService *m_tcpService;
        QString m_host;
        tint m_port;

        QList<MemoryPacket *> m_sQueue;
        QList<MemoryPacket *> m_rQueue;

        bool m_timerFlag;
        time_t m_time;

        bool m_errorFlag;
        bool m_lock;

        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printError(const tchar *strR,const tchar *strE,tint eNo) const;

        virtual MemoryPacket *newIOPacket();
        virtual void freeIOPacket(QList<MemoryPacket *>& queue,tint i);

    signals:

        void onStop();
};

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

