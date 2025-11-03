#include "common/inc/AsynchronousFileReader.h"
#include "common/inc/Log.h"
#include "common/inc/DiskOps.h"

#if defined(OMEGA_MAC_STORE)
#include "common/inc/SBService.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------
// AsynchronousFileReader::IOControlBlock
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

AsynchronousFileReader::IOControlBlock::IOControlBlock()
{
    m_controlBlock = reinterpret_cast<struct aiocb *>(::malloc(sizeof(struct aiocb)));
    if(m_controlBlock!=0)
    {
        ::memset(m_controlBlock,0,sizeof(struct aiocb));
    }
}

//-------------------------------------------------------------------------------------------

AsynchronousFileReader::IOControlBlock::~IOControlBlock()
{
    if(m_controlBlock!=0)
    {
        ::free(m_controlBlock);
        m_controlBlock = 0;
    }
}

//-------------------------------------------------------------------------------------------

struct aiocb *AsynchronousFileReader::IOControlBlock::controlBlock()
{
    return m_controlBlock;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
// AsynchronousFileReader::Request
//-------------------------------------------------------------------------------------------

AsynchronousFileReader::Request::Request() : m_pFile(0)
    ,m_position(0)
    ,m_length(0)
    ,m_readId(-1)
#if defined(OMEGA_POSIX)
    ,m_controlBlock()
#endif
{}

//-------------------------------------------------------------------------------------------

AsynchronousFileReader::Request::Request(AsynchronousFileReader *pFile,tint64 pos,tint len) : m_pFile(pFile)
    ,m_position(pos)
    ,m_length(len)
    ,m_readId(-1)
#if defined(OMEGA_POSIX)
    ,m_controlBlock(new AsynchronousFileReader::IOControlBlock())
#endif
{}

//-------------------------------------------------------------------------------------------

AsynchronousFileReader::Request::Request(const Request& rhs)
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

const AsynchronousFileReader::Request& AsynchronousFileReader::Request::operator = (const Request& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::Request::copy(const Request& rhs)
{
    m_pFile = rhs.m_pFile;
    m_position = rhs.m_position;
    m_length = rhs.m_length;
    m_readId = rhs.m_readId;
#if defined(OMEGA_POSIX)
    m_controlBlock = rhs.m_controlBlock;
#endif
}

//-------------------------------------------------------------------------------------------

AsynchronousFileReader *AsynchronousFileReader::Request::file() const
{
    return m_pFile;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::Request::setFile(AsynchronousFileReader *pFile)
{
    m_pFile = pFile;
}

//-------------------------------------------------------------------------------------------

const tint64& AsynchronousFileReader::Request::position() const
{
    return m_position;
}

//-------------------------------------------------------------------------------------------

const tint& AsynchronousFileReader::Request::length() const
{
    return m_length;
}

//-------------------------------------------------------------------------------------------

tint& AsynchronousFileReader::Request::readId()
{
    return m_readId;
}

//-------------------------------------------------------------------------------------------

const tint& AsynchronousFileReader::Request::readId() const
{
    return m_readId;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

QSharedPointer<AsynchronousFileReader::IOControlBlock>& AsynchronousFileReader::Request::controlBlock()
{
    return m_controlBlock;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
// AsynchronousFileReader::Result
//-------------------------------------------------------------------------------------------

AsynchronousFileReader::Result::Result(int expectedDataSize) : m_pData(new QByteArray())
#if defined(OMEGA_WINDOWS)
    ,m_pOverlapped(new OVERLAPPED)
#endif
{
    m_pData->resize(expectedDataSize);
}

//-------------------------------------------------------------------------------------------

AsynchronousFileReader::Result::Result(const Result& rhs) : m_pData()
#if defined(OMEGA_WINDOWS)
    ,m_pOverlapped()
#endif
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

const AsynchronousFileReader::Result& AsynchronousFileReader::Result::operator = (const Result& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::Result::copy(const Result& rhs)
{
    m_pData = rhs.m_pData;
#if defined(OMEGA_WINDOWS)
    m_pOverlapped = rhs.m_pOverlapped;
#endif
}

//-------------------------------------------------------------------------------------------

QSharedPointer<QByteArray>& AsynchronousFileReader::Result::data()
{
    return m_pData;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WINDOWS)
//-------------------------------------------------------------------------------------------

QSharedPointer<OVERLAPPED>& AsynchronousFileReader::Result::overlapped()
{
    return m_pOverlapped;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
// AsynchronousFileReader::ReaderProcessEvent
//-------------------------------------------------------------------------------------------

AsynchronousFileReader::ProcessorEvent::ProcessorEvent(EventType type) :
    ServiceDataEvent<AsynchronousFileReader::Request>(static_cast<QEvent::Type>(type),(type==AsynchronousFileReader::ProcessorEvent::e_dereferenceEvent || type==AsynchronousFileReader::ProcessorEvent::e_cancelAllPendingEvent))
{}

//-------------------------------------------------------------------------------------------

AsynchronousFileReader::ProcessorEvent::~ProcessorEvent()
{}

//-------------------------------------------------------------------------------------------
// Processor
//-------------------------------------------------------------------------------------------

ServiceEventThread<AsynchronousFileReader::Processor> *AsynchronousFileReader::Processor::m_thread = 0;
QMutex AsynchronousFileReader::Processor::m_idMutex;
tint AsynchronousFileReader::Processor::m_nextId = 1;

//-------------------------------------------------------------------------------------------

AsynchronousFileReader::Processor::Processor(QObject *parent) : ServiceEventAndCondition(parent)
    ,m_timer(0)
    ,m_files()
    ,m_readRequests()
#if defined(OMEGA_WINDOWS)
    ,m_operationsMap()
    ,m_eventHandles()
#elif defined(OMEGA_POSIX)
    ,m_operationsSet()
#endif
{
    m_timer = new QTimer;
}

//-------------------------------------------------------------------------------------------

AsynchronousFileReader::Processor::~Processor()
{
    delete m_timer;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::Processor::printError(const tchar *strR,const tchar *strE) const
{
    Log::g_Log << "AsynchronousFileReader::Processor::" << strR << " - " << strE << c_endl;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::Processor::printError(const tchar *strR,const tchar *strE,tint errCode) const
{
    QString err = QString::fromUtf8(strE) + " " + getOSErrorString(errCode);
    printError(strR,err.toUtf8().constData());
}

//-------------------------------------------------------------------------------------------

QSharedPointer<AsynchronousFileReader::Processor> AsynchronousFileReader::Processor::ref(AsynchronousFileReader *pFile)
{
    QSharedPointer<Processor> pProcessor;

    if(m_thread==0 || !m_thread->isRunning())
    {
        m_thread = new ServiceEventThread<Processor>();
        if(!m_thread->ignite())
        {
            delete m_thread;
            m_thread = 0;
        }
    }
    if(m_thread!=0 && !m_thread->service().isNull())
    {
        ProcessorEvent *evt = new ProcessorEvent(ProcessorEvent::e_referenceEvent);
        evt->data().setFile(pFile);
        QCoreApplication::postEvent(m_thread->service().data(),evt);
        pProcessor = m_thread->service();
    }
    return pProcessor;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::Processor::deref(AsynchronousFileReader *pFile)
{
    if(m_thread!=0)
    {
        ServiceDataWaitCondition<QVariant> *cond = dynamic_cast<ServiceDataWaitCondition<QVariant> *>(getCondition());
        ProcessorEvent *evt = new ProcessorEvent(ProcessorEvent::e_dereferenceEvent);
        evt->data().setFile(pFile);
        QCoreApplication::postEvent(this,evt);
        cond->wait();
        if(cond->data().toInt()==0)
        {
            ServiceEventThread<Processor> *pThread = m_thread;
            m_thread = 0;
            pThread->quit();
            pThread->wait();
            delete pThread;
        }
    }
}

//-------------------------------------------------------------------------------------------

tint AsynchronousFileReader::Processor::queue(const Request& request)
{
    tint ID;

    if(m_thread!=0 && m_thread->isRunning())
    {
        ProcessorEvent *evt = new ProcessorEvent(ProcessorEvent::e_queueEvent);
        evt->data() = request;
        ID = evt->data().readId() = getNextId();
        QCoreApplication::postEvent(this,evt);
    }
    else
    {
        ID = -1;
    }
    return ID;
}

//-------------------------------------------------------------------------------------------

QList<tint> AsynchronousFileReader::Processor::cancelPending(AsynchronousFileReader *pFile)
{
    QList<tint> idList;

    if(m_thread!=0 && m_thread->isRunning())
    {
        ServiceDataWaitCondition<QVariant> *cond = dynamic_cast<ServiceDataWaitCondition<QVariant> *>(getCondition());
        ProcessorEvent *evt = new ProcessorEvent(ProcessorEvent::e_cancelAllPendingEvent);
        evt->data().setFile(pFile);
        QCoreApplication::postEvent(this,evt);
        cond->wait();

        QList<QVariant> list = cond->data().toList();
        for(QList<QVariant>::iterator ppI=list.begin();ppI!=list.end();ppI++)
        {
            idList.append((*ppI).toInt());
        }
    }
    return idList;
}

//-------------------------------------------------------------------------------------------

tint AsynchronousFileReader::Processor::getNextId()
{
    tint nID;

    m_idMutex.lock();
    nID = m_nextId;
    m_nextId++;
    if(m_nextId < 0)
    {
        m_nextId++;
    }
    m_idMutex.unlock();
    return nID;
}

//-------------------------------------------------------------------------------------------

ServiceWaitCondition *AsynchronousFileReader::Processor::newCondition()
{
    return new ServiceDataWaitCondition<QVariant>();
}

//-------------------------------------------------------------------------------------------

bool AsynchronousFileReader::Processor::processEvent(ServiceEvent *evt)
{
    bool res = false;
    ProcessorEvent *pEvent = dynamic_cast<ProcessorEvent *>(evt);

    if(pEvent!=0)
    {
        Request& request = pEvent->data();

        if(static_cast<ProcessorEvent::EventType>(pEvent->type()) == ProcessorEvent::e_referenceEvent)
        {
            if(m_files.isEmpty())
            {
                connect(m_timer,SIGNAL(timeout()),this,SLOT(onTimer()));
                m_timer->start(1);
            }
            m_files.insert(request.file());
            res = true;
        }
        else if(static_cast<ProcessorEvent::EventType>(pEvent->type()) == ProcessorEvent::e_dereferenceEvent)
        {
            ServiceDataWaitCondition<QVariant> *cond = dynamic_cast<ServiceDataWaitCondition<QVariant> *>(getCondition(evt));
            QSet<AsynchronousFileReader *>::iterator ppI = m_files.find(request.file());

            if(ppI!=m_files.end())
            {
                cancelIOForFile(request.file());
                m_files.erase(ppI);
            }
            else
            {
                printError("processEvent","Dereferencing for file instance that is not currently referenced.");
            }
            cond->data() = QVariant(static_cast<tint>(m_files.size()));
            if(m_files.isEmpty())
            {
                m_timer->stop();
                disconnect(m_timer,SIGNAL(timeout()),this,SLOT(onTimer()));
            }
            res = true;
        }
        else if(static_cast<ProcessorEvent::EventType>(pEvent->type()) == ProcessorEvent::e_queueEvent)
        {
            m_readRequests.append(request);
            res = true;
        }
        else if(static_cast<ProcessorEvent::EventType>(pEvent->type()) == ProcessorEvent::e_cancelAllPendingEvent)
        {
            QList<QVariant> cancelIDList;
            ServiceDataWaitCondition<QVariant> *cond = dynamic_cast<ServiceDataWaitCondition<QVariant> *>(getCondition(evt));
            QSet<AsynchronousFileReader *>::iterator ppI = m_files.find(request.file());

            if(ppI!=m_files.end())
            {
                cancelIDList = cancelPendingIOForFile(request.file());
            }
            cond->data() = QVariant(cancelIDList);

            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QList<AsynchronousFileReader::Request> AsynchronousFileReader::Processor::cancelIOOnFile(AsynchronousFileReader *pFile)
{
    QList<Request> cancelList;

    QList<Request>::iterator ppI = m_readRequests.begin();
    while(ppI!=m_readRequests.end())
    {
        Request& request = *ppI;
        if(request.file() == pFile)
        {
            cancelList.append(request);
            ppI = m_readRequests.erase(ppI);
        }
        else
        {
            ++ppI;
        }
    }

#if defined(OMEGA_WINDOWS)
    pFile->cancelAllPendingIO();

    QMap<HANDLE,Request>::iterator ppJ = m_operationsMap.begin();
    while(ppJ!=m_operationsMap.end())
    {
        Request& request = ppJ.value();
        if(request.file() == pFile)
        {
            cancelList.append(request);
            freeEvent(ppJ.key());
            ppJ = m_operationsMap.erase(ppJ);
        }
        else
        {
            ++ppJ;
        }
    }

#elif defined(OMEGA_POSIX)
    QVector<Request> fileOps;
    QVector<Request>::iterator ppJ = m_operationsSet.begin();
    while(ppJ!=m_operationsSet.end())
    {
        Request& request = *ppJ;
        if(request.file() == pFile)
        {
            cancelList.append(request);
            fileOps.append(request);
            ppJ = m_operationsSet.erase(ppJ);
        }
        else
        {
            ppJ++;
        }
    }

    pFile->cancelAllPendingIO(fileOps);
#endif

    return cancelList;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::Processor::cancelIOForFile(AsynchronousFileReader *pFile)
{
    QList<Request> errorList = cancelIOOnFile(pFile);
    for(QList<Request>::iterator ppI=errorList.begin();ppI!=errorList.end();++ppI)
    {
        Request& request = *ppI;
        request.file()->emitOnError(request.readId());
    }
}

//-------------------------------------------------------------------------------------------

QList<QVariant> AsynchronousFileReader::Processor::cancelPendingIOForFile(AsynchronousFileReader *pFile)
{
    QList<QVariant> cancelList;
    QList<Request> list = cancelIOOnFile(pFile);

    for(QList<Request>::iterator ppI=list.begin();ppI!=list.end();++ppI)
    {
        Request& request = *ppI;
        cancelList.append(QVariant(request.readId()));
    }
    return cancelList;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WINDOWS)
//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::Processor::onTimer()
{
    HANDLE handles[c_maxSimultaneousReads];
    bool loop = true;

    enqueuePendingReads();

    while(loop && m_operationsMap.size() > 0)
    {
        tint i,idx;
        QMap<HANDLE,Request>::iterator ppI;

        i = 0;
        for(ppI=m_operationsMap.begin();ppI!=m_operationsMap.end();++ppI)
        {
            handles[i] = ppI.key();
            i++;
        }

        DWORD wRes = ::WaitForMultipleObjects(m_operationsMap.size(),handles,FALSE,0);
        if(static_cast<tint>(wRes - WAIT_OBJECT_0) >= 0 && static_cast<tint>(wRes - WAIT_OBJECT_0) < m_operationsMap.size())
        {
            idx = static_cast<tint>(wRes - WAIT_OBJECT_0);
            ppI = m_operationsMap.find(handles[idx]);
            if(ppI!=m_operationsMap.end())
            {
                Request& request = ppI.value();
                request.file()->readOpComplete(request);
                m_operationsMap.erase(ppI);
            }
        }
        else if(static_cast<tint>(wRes - WAIT_ABANDONED_0) >= 0 && static_cast<tint>(wRes - WAIT_ABANDONED_0) < m_operationsMap.size())
        {
            printError("onTimer","Unexpected abandoned on read event signal");

            idx = static_cast<tint>(wRes - WAIT_ABANDONED_0);
            ppI = m_operationsMap.find(handles[idx]);
            if(ppI!=m_operationsMap.end())
            {
                Request& request = ppI.value();
                request.file()->emitOnError(request.readId());
                m_operationsMap.erase(ppI);
            }
        }
        else if(wRes==WAIT_TIMEOUT)
        {
            loop = false;
        }
        else
        {
            printError("onTimer","Error polling read event signals.",static_cast<tint>(::GetLastError()));
            loop = false;
        }
    }

    pollCompleted();
}

//-------------------------------------------------------------------------------------------

HANDLE AsynchronousFileReader::Processor::getEvent()
{
    HANDLE hEvent;

    if(m_eventHandles.isEmpty())
    {
        hEvent = ::CreateEvent(0,TRUE,FALSE,0);
        if(hEvent==0)
        {
            printError("getEvent","Failed to create asychronous event",static_cast<tint>(::GetLastError()));
        }
    }
    else
    {
        hEvent = m_eventHandles.pop();
        if(::ResetEvent(hEvent)==0)
        {
            printError("getEvent","Failed to reset asychronous event.",static_cast<tint>(::GetLastError()));
            ::CloseHandle(hEvent);
            hEvent = getEvent();
        }
    }
    return hEvent;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::Processor::freeEvent(HANDLE hEvent)
{
    if(hEvent!=0)
    {
        m_eventHandles.push(hEvent);
    }
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::Processor::enqueuePendingReads()
{
    while(m_operationsMap.size() < c_maxSimultaneousReads && !m_readRequests.isEmpty())
    {
        Request request = m_readRequests.takeFirst();
        HANDLE hEvent = getEvent();

        if(hEvent!=0)
        {
            if(request.file()->doReadOperation(hEvent,request))
            {
                m_operationsMap.insert(hEvent,request);
            }
        }
        else
        {
            QString err = "Failed to create event for associated read for file '" + request.file()->name() + "'";
            printError("enqueuePendingReads",err.toUtf8().constData());
            request.file()->emitOnError(request.readId());
        }
    }
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::Processor::onTimer()
{
    enqueuePendingReads();

    if(m_operationsSet.size() > 0)
    {
        QVector<Request>::iterator ppI;

        ppI = m_operationsSet.begin();
        while(ppI!=m_operationsSet.end())
        {
            Request& request = *ppI;
            if(request.file()->readOpComplete(request))
            {
                ppI = m_operationsSet.erase(ppI);
            }
            else
            {
                ppI++;
            }
        }
    }

    pollCompleted();
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::Processor::enqueuePendingReads()
{
    while(m_operationsSet.size() < c_maxSimultaneousReads && !m_readRequests.isEmpty())
    {
        Request request = m_readRequests.takeFirst();

        if(request.file()->doReadOperation(request))
        {
            m_operationsSet.append(request);
        }
    }
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::Processor::pollCompleted()
{
    for(QSet<AsynchronousFileReader *>::iterator ppI=m_files.begin();ppI!=m_files.end();++ppI)
    {
        AsynchronousFileReader *pFile = *ppI;
        pFile->signalCompleteAsRequired();
    }
}

//-------------------------------------------------------------------------------------------
// AsynchronousFileReader
//-------------------------------------------------------------------------------------------

AsynchronousFileReader::AsynchronousFileReader(QObject *parent) : QObject(parent)
    ,m_size(0)
    ,m_name()
    ,m_results()
    ,m_pProcessor()
#if defined(OMEGA_WINDOWS)
    ,m_file(INVALID_HANDLE_VALUE)
#elif defined(OMEGA_POSIX)
    ,m_file(-1)
#endif
#if defined(OMEGA_MAC_STORE)
    ,m_checkOutFlag(true)
#endif
    ,m_mutex()
    ,m_resultMap()
{}

//-------------------------------------------------------------------------------------------

AsynchronousFileReader::~AsynchronousFileReader()
{
    AsynchronousFileReader::close();
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::printError(const tchar *strR,const tchar *strE) const
{
    Log::g_Log << "AsynchronousFileReader::" << strR << " - " << strE << c_endl;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::printError(const tchar *strR,const tchar *strE,tint errCode) const
{
    QString err = QString::fromUtf8(strE) + " " + getOSErrorString(errCode);
    printError(strR,err.toUtf8().constData());
}

//-------------------------------------------------------------------------------------------

const QString& AsynchronousFileReader::name() const
{
    return m_name;
}

//-------------------------------------------------------------------------------------------

bool AsynchronousFileReader::open(const QString& name)
{
    bool res = false;

    close();

    if(!name.isEmpty())
    {
#if defined(OMEGA_MAC_STORE)
        if(m_checkOutFlag)
        {
            SBBookmarkPtr sbBookmark = SBBookmark::get();
            if(!sbBookmark->has(name,true))
            {
                sbBookmark->add(name,true);
            }
            sbBookmark->checkOut(name,true);
        }
#endif

        if(DiskOps::exist(name))
        {
#if defined(OMEGA_WINDOWS)
            LPCWSTR wStr = reinterpret_cast<LPCWSTR>(name.utf16());

            m_file = ::CreateFileW(wStr,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_OVERLAPPED, 0);
            if(m_file != INVALID_HANDLE_VALUE)
#elif defined(OMEGA_POSIX)
            m_file = ::open(name.toUtf8().constData(),O_RDONLY);
            if(m_file >= 0)
#endif
            {
                m_name = name;
                m_size = queryFileSize();
                if(m_size >= 0)
                {
                    m_pProcessor = Processor::ref(this);
                    if(!m_pProcessor.isNull())
                    {
                        res = true;
                    }
                    else
                    {
                        printError("open","Failed to get reference to processing thread");
                    }
                }
                else
                {
                    QString err = "Failed to get size of file '" + name + "'";
                    printError("open",err.toUtf8().constData());
                }
            }
            else
            {
                QString err = "Failed to open file '" + name + "' for reading.";
#if defined(OMEGA_WINDOWS)
                printError("open",err.toUtf8().constData(),static_cast<DWORD>(::GetLastError()));
#elif defined(OMEGA_POSIX)
                printError("open",err.toUtf8().constData(),errno);
#endif
            }
        }
        else
        {
            QString err = "File '" + name + "' does not exist";
            printError("open",err.toUtf8().constData());
        }
    }
    else
    {
        printError("open","No name given for file");
    }

    if(!res)
    {
        close();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::close()
{
    if(!m_pProcessor.isNull())
    {
        m_pProcessor->deref(this);
        m_pProcessor.clear();
    }

#if defined(OMEGA_WINDOWS)
    if(m_file!=INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_file);
        m_file = INVALID_HANDLE_VALUE;
    }
#elif defined(OMEGA_POSIX)
    if(m_file >= 0)
    {
        ::close(m_file);
        m_file = -1;
    }
#endif

#if defined(OMEGA_MAC_STORE)
    if(m_checkOutFlag)
    {
        SBBookmarkPtr sbBookmark = SBBookmark::get();
        sbBookmark->checkIn(m_name,true);
    }
#endif

    m_size = 0;
    m_name = "";
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WINDOWS)
//-------------------------------------------------------------------------------------------

tint64 AsynchronousFileReader::queryFileSize()
{
    tint64 s = -1;

    if(m_file!=INVALID_HANDLE_VALUE)
    {
        DWORD high,low;

        high = 0;
        low = ::GetFileSize(m_file,&high);
        if(low==INVALID_FILE_SIZE && ::GetLastError()!=NO_ERROR)
        {
            QString err = "Failed to obtain file size for '" + m_name + "'";
            printError("size",err.toUtf8().constData(),static_cast<int>(::GetLastError()));
        }
        else
        {
            tuint64 v;
            v  = (static_cast<tuint64>(high) << 32) & 0xffffffff00000000ULL;
            v |= (static_cast<tuint64>(low )      ) & 0x00000000ffffffffULL;
            s = static_cast<tint64>(v);
        }
    }
    else
    {
        printError("size","No file open to get size from");
    }
    return s;
}

//-------------------------------------------------------------------------------------------
// Called from the ReaderProcessThread
// doReadOperation - performs the ReadFile call and assigns the memory buffer
// readOpComplete - calls GetOverlappedResult and calls the onReadComplete signal
//-------------------------------------------------------------------------------------------

bool AsynchronousFileReader::doReadOperation(HANDLE hEvent,const Request& request)
{
    bool res = false;

    if(m_file!=INVALID_HANDLE_VALUE)
    {
        LPVOID pData;
        OVERLAPPED *overlapped;
        Result result(request.length());

        overlapped = result.overlapped().data();
        ::memset(overlapped,0,sizeof(OVERLAPPED));
        overlapped->Offset = static_cast<DWORD>(request.position() & 0x00000000ffffffffULL);
        overlapped->OffsetHigh = static_cast<DWORD>((request.position() >> 32) & 0x00000000ffffffffULL);
        overlapped->hEvent = hEvent;

        pData = reinterpret_cast<LPVOID>(result.data()->data());

        if(::ReadFile(m_file,pData,static_cast<DWORD>(request.length()),0,overlapped)==FALSE)
        {
            DWORD errCode = ::GetLastError();
            if(errCode!=ERROR_IO_PENDING)
            {
                printError("doReadOperation","Error performing asynchronous read opertation",static_cast<tint>(errCode));
                emitOnError(request.readId());
            }
            else
            {
                m_results.insert(request.readId(),result);
                res = true;
            }
        }
        else
        {
            m_results.insert(request.readId(),result);
            res = true;
        }
    }
    else
    {
        printError("doReadOperation","File descriptor is closed");
        emitOnError(request.readId());
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool AsynchronousFileReader::readOpComplete(Request& request)
{
    QMap<tint,Result>::iterator ppI;

    ppI = m_results.find(request.readId());
    if(ppI != m_results.end())
    {
        DWORD len = 0;
        OVERLAPPED *overlapped;
        Result& result = ppI.value();

        overlapped = result.overlapped().data();
        if(GetOverlappedResult(m_file,overlapped,&len,TRUE)!=0)
        {
            QSharedPointer<QByteArray> pData(result.data());

            if(pData->size() > static_cast<tint>(len))
            {
                pData->truncate(static_cast<tint>(len));
            }
            emitOnReadComplete(request.readId(),pData);
        }
        else
        {
            printError("readOpComplete","Error in asychronous read operation",static_cast<tint>(::GetLastError()));
            emitOnError(request.readId());
        }
        m_results.erase(ppI);
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::cancelAllPendingIO()
{
    if(!m_results.isEmpty())
    {
        if(::CancelIo(m_file)==0)
        {
            printError("cancelAllPending","Error cancelling pending read operations",static_cast<tint>(::GetLastError()));
        }
        m_results.clear();
    }
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

tint64 AsynchronousFileReader::queryFileSize()
{
    tint64 s = -1;

    if(m_file >= 0)
    {
        off_t val;

        val = ::lseek(m_file,0,SEEK_END);
        if(val != -1)
        {
            if(::lseek(m_file,0,SEEK_SET) == 0)
            {
                s = static_cast<tint64>(val);
            }
            else
            {
                QString err = "Error resetting file position back to start of file '" + m_name + "'";
                printError("queryFileSize",err.toUtf8().constData(),errno);
            }
        }
        else
        {
            QString err = "Failed to seek to end of file position of '" + m_name + "'";
            printError("queryFileSize",err.toUtf8().constData(),errno);
        }
    }
    else
    {
        printError("size","No file open to get size from");
    }
    return s;
}

//-------------------------------------------------------------------------------------------

bool AsynchronousFileReader::doReadOperation(Request& request)
{
    bool res = false;

    if(m_file >= 0)
    {
        struct aiocb *cb = request.controlBlock()->controlBlock();

        if(cb!=0)
        {
            Result result(request.length());

            ::memset(cb,0,sizeof(struct aiocb));
            cb->aio_fildes = m_file;
            cb->aio_offset = static_cast<off_t>(request.position());
            cb->aio_buf = result.data()->data();
            cb->aio_nbytes = request.length();
            cb->aio_sigevent.sigev_notify = SIGEV_NONE;
            cb->aio_sigevent.sigev_value.sival_int = request.readId();

            if(::aio_read(cb)==0)
            {
                m_results.insert(request.readId(),result);
                res = true;
            }
            else
            {
                QString err = "Error queueing asynchronise read on file '" + m_name + "'";
                printError("doReadOperation",err.toUtf8().constData(),errno);
                emitOnError(request.readId());
            }
        }
        else
        {
            printError("doReadOperation","Expected I/O control block has not been allocated");
            emitOnError(request.readId());
        }
    }
    else
    {
        printError("doReadOperation","File descriptor is closed");
        emitOnError(request.readId());
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool AsynchronousFileReader::readOpComplete(Request& request)
{
    tint ioResult;
    bool isComplete;
    struct aiocb *cb = request.controlBlock()->controlBlock();

    ioResult = ::aio_error(cb);
    if(ioResult!=EINPROGRESS)
    {
        QMap<tint,Result>::iterator ppI;

        ioResult = ::aio_return(cb);
        if(ioResult>=0)
        {
            ppI = m_results.find(request.readId());
            if(ppI!=m_results.end())
            {
                Result& result = ppI.value();

                if(ioResult <= result.data()->size())
                {
                    if(ioResult < result.data()->size())
                    {
                        result.data()->truncate(ioResult);
                    }
                    emitOnReadComplete(request.readId(),result.data());
                }
                else
                {
                    QString err = "Number of bytes given for read from '" + m_name + "' exceeds requested size";
                    printError("processIfComplete",err.toUtf8().constData(),ioResult);
                    emitOnError(request.readId());
                }
            }
            else
            {
                QString err = "Failed to find expected result for read operation on file '" + m_name + "'";
                printError("processIfComplete",err.toUtf8().constData(),ioResult);
                emitOnError(request.readId());
            }
        }
        else
        {
            QString err = "Read operation failed on file '" + m_name + "'";
            printError("processIfComplete",err.toUtf8().constData(),ioResult);
            emitOnError(request.readId());
        }
        isComplete = true;

        ppI = m_results.find(request.readId());
        if(ppI!=m_results.end())
        {
            m_results.erase(ppI);
        }
    }
    else
    {
        isComplete = false;
    }
    return isComplete;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::cancelAllPendingIO(QVector<Request>& requests)
{
    tint count = 0;

    ::aio_cancel(m_file,0);

    while(!requests.isEmpty() && count < 5000)
    {
        QVector<Request>::iterator ppI = requests.begin();
        while(ppI!=requests.end())
        {
            Request& request = *ppI;
            struct aiocb *cb = request.controlBlock()->controlBlock();

            if(::aio_error(cb)==EINPROGRESS)
            {
                ppI++;
            }
            else
            {
                ::aio_return(cb);
                ppI = requests.erase(ppI);
            }
        }

        if(!requests.isEmpty())
        {
            struct aiocb **cbList = reinterpret_cast<struct aiocb **>(::malloc(requests.size() * sizeof(struct aiocb *)));
            if(cbList!=0)
            {
                int idx;
                struct timespec timeout;

                for(idx=0,ppI=requests.begin();ppI!=requests.end();ppI++,idx++)
                {
                    Request& request = *ppI;
                    cbList[idx] = request.controlBlock()->controlBlock();
                }

                timeout.tv_sec = 0;
                timeout.tv_nsec = 10000L; // 10 microseconds
                ::aio_suspend(cbList,requests.size(),&timeout);
                ::free(cbList);
            }
        }
        count++;
    }

    m_results.clear();
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::emitOnReadComplete(tint readId,QSharedPointer<QByteArray>& pData)
{
    m_mutex.lock();
    m_resultMap.insert(readId,pData);
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::emitOnError(tint readId)
{
    m_mutex.lock();
    m_resultMap.insert(readId,QSharedPointer<QByteArray>());
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::signalCompleteAsRequired()
{
    bool eFlag;

    m_mutex.lock();
    eFlag = (!m_resultMap.isEmpty());
    m_mutex.unlock();

    if(eFlag)
    {
        Q_EMIT completed();
    }
}

//-------------------------------------------------------------------------------------------

QList<int> AsynchronousFileReader::hasCompleted()
{
    m_mutex.lock();
    QList<int> ids = m_resultMap.keys();
    m_mutex.unlock();
    return ids;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<QByteArray> AsynchronousFileReader::result(tint readId)
{
    QSharedPointer<QByteArray> pData;
    QMap<int,QSharedPointer<QByteArray> >::iterator ppI;

    m_mutex.lock();
    ppI = m_resultMap.find(readId);
    if(ppI != m_resultMap.end())
    {
        pData = ppI.value();
        m_resultMap.erase(ppI);
    }
    m_mutex.unlock();
    return pData;
}

//-------------------------------------------------------------------------------------------
// -1 => error, 0 => async read operation cannot be executed, >=1 => readId from complete signal
//-------------------------------------------------------------------------------------------

tint AsynchronousFileReader::read(tint64 fromPosition,tint len)
{
    tint res;

    if(fromPosition>=0 && fromPosition<m_size && len>0)
    {
        // Adjust length to match size of file.
        tint64 toPosition = fromPosition + len;
        if(toPosition > m_size)
        {
            len = static_cast<tint>(m_size - fromPosition);
        }

        Request request(this,fromPosition,len);
        res = m_pProcessor->queue(request);
    }
    else
    {
        res = 0;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint AsynchronousFileReader::read(tint fromPosition,tint len)
{
    return read(static_cast<tint64>(fromPosition),len);
}

//-------------------------------------------------------------------------------------------

tint AsynchronousFileReader::size() const
{
    return static_cast<tint>(m_size);
}

//-------------------------------------------------------------------------------------------

tint64 AsynchronousFileReader::size64() const
{
    return m_size;
}

//-------------------------------------------------------------------------------------------

QList<int> AsynchronousFileReader::cancelAllPending()
{
    return m_pProcessor->cancelPending(this);
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MAC_STORE)
//-------------------------------------------------------------------------------------------

void AsynchronousFileReader::setCheckOutFlag(bool flag)
{
    m_checkOutFlag = flag;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
