//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_ASYNCHRONOUSFILEREADER_H
#define __OMEGA_COMMON_ASYNCHRONOUSFILEREADER_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BIOStream.h"
#include "common/inc/CommonFunctions.h"

#if defined(OMEGA_POSIX)
#include <aio.h>
#endif

#include <QObject>
#include <QSharedPointer>
#include <QThread>
#include <QList>
#include <QMap>
#include <QSet>
#include <QVector>
#include <QWaitCondition>
#include <QMutex>
#include <QStack>
#include <QTimer>
#include <QCoreApplication>
#include <QVariant>

#include "common/inc/ServiceEvent.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

#define c_maxSimultaneousReads 16

typedef QSharedPointer<QByteArray> QByteArraySPtr;

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT AsynchronousFileReader : public QObject
{
    public:
        Q_OBJECT
    
    public:

#if defined(OMEGA_POSIX)
        class IOControlBlock;
#endif        
        class Request;
        class Result;
        class ProcessorEvent;
        class Processor;
        
        friend class Processor;
        
    public:
        AsynchronousFileReader(QObject *parent = 0);
        virtual ~AsynchronousFileReader();
        
        virtual const QString& name() const;
        
        virtual bool open(const QString& name);
        virtual void close();
        
        // -1 => error, 0 => async read operation cannot be executed, >=1 => readId from complete signal
        virtual tint read(tint fromPosition,tint len);
        virtual tint read(tint64 fromPosition,tint len);
        
        virtual tint size() const;
        virtual tint64 size64() const;

#if defined(OMEGA_MAC_STORE)
        virtual void setCheckOutFlag(bool flag);
#endif

        virtual QList<int> hasCompleted();
        virtual QSharedPointer<QByteArray> result(tint readId);
        
        virtual QList<int> cancelAllPending();
        
    protected:
    
        tint64 m_size;
        QString m_name;
        QMap<tint,Result> m_results;
        QSharedPointer<Processor> m_pProcessor;
        
#if defined(OMEGA_WINDOWS)
        HANDLE m_file;
#elif defined(OMEGA_POSIX)
        int m_file;
#endif

#if defined(OMEGA_MAC_STORE)
        bool m_checkOutFlag;
#endif

        QMutex m_mutex;
        QMap<int,QSharedPointer<QByteArray> > m_resultMap;

        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printError(const tchar *strR,const tchar *strE,tint errCode) const;

        virtual tint64 queryFileSize();

#if defined(OMEGA_WINDOWS)
        virtual bool doReadOperation(HANDLE hEvent,const Request& request);
        virtual void cancelAllPendingIO();
#elif defined(OMEGA_POSIX)
        virtual bool doReadOperation(Request& request);
        virtual void cancelAllPendingIO(QVector<Request>& requests);
#endif
        virtual bool readOpComplete(Request& request);
        
        virtual void emitOnReadComplete(tint readId,QSharedPointer<QByteArray>& pData);
        virtual void emitOnError(tint readId);
        virtual void signalCompleteAsRequired();
    
    signals:        
        void completed();
};

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT AsynchronousFileReader::IOControlBlock
{
    public:
        IOControlBlock();
        virtual ~IOControlBlock();
                
        struct aiocb *controlBlock();
                
    private:
        struct aiocb *m_controlBlock;
};

//-------------------------------------------------------------------------------------------        
#endif
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT AsynchronousFileReader::Request
{
    public:
        Request();
        Request(AsynchronousFileReader *pFile,tint64 pos,tint len);
        Request(const Request& rhs);
        
        const Request& operator = (const Request& rhs);
        
        AsynchronousFileReader *file() const;
        void setFile(AsynchronousFileReader *pFile);
        
        const tint64& position() const;
        const tint& length() const;
        
        tint& readId();
        const tint& readId() const;

#if defined(OMEGA_POSIX)
        QSharedPointer<IOControlBlock>& controlBlock();
#endif
    protected:
                
        AsynchronousFileReader *m_pFile;
        tint64 m_position;
        tint m_length;
        tint m_readId;
#if defined(OMEGA_POSIX)
        QSharedPointer<IOControlBlock> m_controlBlock;
#endif
        
        void copy(const Request& rhs);
};

//-------------------------------------------------------------------------------------------
        
class COMMON_EXPORT AsynchronousFileReader::Result
{
    public:
        Result(int expectedDataSize);
        Result(const Result& rhs);
        
        const Result& operator = (const Result& rhs);
        
        QSharedPointer<QByteArray>& data();
#if defined(OMEGA_WINDOWS)
        QSharedPointer<OVERLAPPED>& overlapped();
#endif
    protected:
        QSharedPointer<QByteArray> m_pData;
#if defined(OMEGA_WINDOWS)
        QSharedPointer<OVERLAPPED> m_pOverlapped;
#endif
        void copy(const Result& rhs);
};

//-------------------------------------------------------------------------------------------
    
class COMMON_EXPORT AsynchronousFileReader::ProcessorEvent : public ServiceDataEvent<Request>
{
    public:
        typedef enum
        {
            e_referenceEvent = QEvent::User + 1755,
            e_dereferenceEvent,
            e_queueEvent,
            e_cancelAllPendingEvent
        } EventType;
        
    public:
        ProcessorEvent(EventType type);
        virtual ~ProcessorEvent();
};

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT AsynchronousFileReader::Processor : public ServiceEventAndCondition
{
    public:
        Q_OBJECT

    public:
        Processor(QObject *parent = 0);
        virtual ~Processor();

        static QSharedPointer<Processor> ref(AsynchronousFileReader *pFile);
        virtual void deref(AsynchronousFileReader *pFile);
        virtual tint queue(const Request& request);
        virtual QList<tint> cancelPending(AsynchronousFileReader *pFile);

    protected:

        static ServiceEventThread<Processor> *m_thread;
        static QMutex m_idMutex;
        static tint m_nextId;

        QTimer *m_timer;
    
        QSet<AsynchronousFileReader *> m_files;
        QList<Request> m_readRequests;

#if defined(OMEGA_WINDOWS)
        QMap<HANDLE,Request> m_operationsMap;
        QStack<HANDLE> m_eventHandles;
#elif defined(OMEGA_POSIX)
        QVector<Request> m_operationsSet;
#endif

        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printError(const tchar *strR,const tchar *strE,tint errCode) const;

        virtual tint getNextId();

        virtual ServiceWaitCondition *newCondition();
        virtual bool processEvent(ServiceEvent *evt);

#if defined(OMEGA_WINDOWS)
        virtual HANDLE getEvent();
        virtual void freeEvent(HANDLE hEvent);
#endif
        virtual void enqueuePendingReads();
        
        virtual void cancelIOForFile(AsynchronousFileReader *pFile);
        virtual QList<Request> cancelIOOnFile(AsynchronousFileReader *pFile);
        virtual QList<QVariant> cancelPendingIOForFile(AsynchronousFileReader *pFile);
        
        virtual void pollCompleted();
        
    protected slots:
        void onTimer();
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
