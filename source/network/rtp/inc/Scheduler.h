//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_SCHEDULER_H
#define __OMEGA_NETWORK_RTP_SCHEDULER_H
//-------------------------------------------------------------------------------------------

#include "network/inc/UDPRead.h"
#include "network/inc/UDPWrite.h"
#include "network/rtp/inc/RTPPacket.h"
#include "network/rtp/inc/RTCPPacketSDES.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

class SchedulerEvent : public QEvent
{
    public:
    
        typedef enum
        {
            e_newSenderEvent = QEvent::User + 1010,
            e_newReceiverEvent,
            e_completeEvent,
            e_resyncEvent
        } SchedulerEventType;
        
    public:
        SchedulerEvent(SchedulerEventType t);
        
        Qt::HANDLE threadId();
        
        const QString& resource() const;
        void resource(const QString& x);
        const QString& host() const;
        void host(const QString& x);
        tint port() const;
        void port(tint x);
        const QString& rHost() const;
        void rHost(const QString& x);
        tint rPort() const;
        void rPort(tint x);
        Session *session() const;
        void session(Session *x);
        
    protected:
    
        Qt::HANDLE m_threadId;
        QString m_resource;
        QString m_host;
        tint m_port;
        QString m_rHost;
        tint m_rPort;
        Session *m_session;
};

//-------------------------------------------------------------------------------------------

class NETWORKRTP_EXPORT Scheduler : public Service
{
    public:
        Q_OBJECT
        
    public:
        Scheduler(QObject *parent = 0);
        virtual ~Scheduler();
                
        // SDES information for RTP stack.
        static const SDESBlock& idInfo();
        static void setidInfo(const SDESBlock& info);
        
        // external interface.
        // start new sender session for given resource.
        virtual Session *newSender(const QString& resource,const QString& host,tint port,const QString& rHost,tint rPort);
        // start new sender reciever for given resource.
        virtual Session *newReceiver(const QString& resource,const QString& host,tint port,const QString& rHost,tint rPort);
        // complete session - this destroys the object.
        virtual void complete(Session *session);
        // resync - resynchronise the given session to the receiving sources.
        virtual void resync(Session *session);
        
        virtual bool start();
        virtual void stop();

        virtual bool process();
        
    protected:
        
        static SDESBlock m_sdesInfo;
        
        tint m_timeoutCount;
        
        QSet<Session *> m_sessionSet;
        
        static QString getFileExtension(const QString& name);
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
                
        virtual bool event(QEvent *e);
        virtual void processCustomEvent(SchedulerEvent *e,void *results);
        
        virtual Session *onNewSender(const QString& resource,const QString& host,tint port,const QString& rHost,tint rPort);
        virtual Session *onNewReceiver(const QString& resource,const QString& host,tint port,const QString& rHost,tint rPort);
        virtual void onComplete(Session *session);
        virtual void onResync(Session *session);
};

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

