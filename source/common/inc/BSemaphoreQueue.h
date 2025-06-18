//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_BSEMAPHOREQUEUE_H
#define __OMEGA_COMMON_BSEMAPHOREQUEUE_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"
#include "common/inc/BOQueueTree.h"
#include "common/inc/Semaphore.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

typedef struct
{
    HANDLE cond;
    CRITICAL_SECTION mutex;
} BSemaphoreItem;

//-------------------------------------------------------------------------------------------            
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

typedef struct s_BSemaphoreItem
{
    pthread_cond_t cond;
    pthread_mutex_t mutex;
} BSemaphoreItem;

//-------------------------------------------------------------------------------------------        
#endif
//-------------------------------------------------------------------------------------------

const tint c_BSemaphoreQueueTimeoutDefault = 200; // default timeout of 200 milliseconds.

//-------------------------------------------------------------------------------------------

template <class X> class BSemaphoreQueue
{

    public:
        BSemaphoreQueue();
        BSemaphoreQueue(tint timeout);
        
        ~BSemaphoreQueue();
        
        void Up(X *x);
        
        X *Down();
                
        tint Count() const;
    
    protected:
    
        typedef struct s_RSemaphoreItem
        {
            BSemaphoreItem unit;
            X *item;
        } RSemaphoreItem;
    
#if defined(OMEGA_WIN32)
        mutable CRITICAL_SECTION m_Mutex;
#elif defined(OMEGA_POSIX)
        mutable pthread_mutex_t m_Mutex;
#endif

        tint m_Timeout;
    
        BOQueueTree<BSemaphoreItem *> m_List;
        BOQueueTree<X *> m_Queue;
        
        void lock() const;
        void unlock() const;
        
        BSemaphoreItem *getItem();
        void freeItem(BSemaphoreItem *item);
        
        void lockItem(BSemaphoreItem *item);
        void unlockItem(BSemaphoreItem *item);
};

//-------------------------------------------------------------------------------------------

template <class X> BSemaphoreQueue<X>::BSemaphoreQueue() : m_Timeout(c_BSemaphoreQueueTimeoutDefault),
    m_List(),
    m_Queue()
{
#if defined(OMEGA_WIN32)
    ::InitializeCriticalSection(&m_Mutex);
#elif defined(OMEGA_POSIX)
    ::pthread_mutex_init(&m_Mutex,0);
#endif
}

//-------------------------------------------------------------------------------------------

template <class X> BSemaphoreQueue<X>::BSemaphoreQueue(tint timeout) : m_Timeout(timeout),
    m_List(),
    m_Queue()
{
#if defined(OMEGA_WIN32)
    ::InitializeCriticalSection(&m_Mutex);
#elif defined(OMEGA_POSIX)
    ::pthread_mutex_init(&m_Mutex,0);
#endif
}

//-------------------------------------------------------------------------------------------

template <class X> BSemaphoreQueue<X>::~BSemaphoreQueue()
{
    try
    {
        RSemaphoreItem *item;
        
        lock();
        while(item=reinterpret_cast<RSemaphoreItem *>(m_List.Find(0)),item!=0)
        {
            m_List.Delete(0);
            
            lockItem(reinterpret_cast<BSemaphoreItem *>(item));
            unlockItem(reinterpret_cast<BSemaphoreItem *>(item));
            
#if defined(OMEGA_WIN32)
            ::SetEvent(item->unit.cond);
#elif defined(OMEGA_POSIX)
            ::pthread_cond_signal(&item->unit.cond);
#endif
            freeItem(reinterpret_cast<BSemaphoreItem *>(item));
        }
        unlock();
                
#if defined(OMEGA_WIN32)
        ::DeleteCriticalSection(&m_Mutex);
#elif defined(OMEGA_POSIX)
        ::pthread_mutex_destroy(&m_Mutex);
#endif
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

template <class X> void BSemaphoreQueue<X>::lock() const
{
#if defined(OMEGA_WIN32)
    ::EnterCriticalSection(&m_Mutex);
#elif defined(OMEGA_POSIX)
    ::pthread_mutex_lock(&m_Mutex);
#endif    
}

//-------------------------------------------------------------------------------------------

template <class X> void BSemaphoreQueue<X>::unlock() const
{
#if defined(OMEGA_WIN32)
    ::LeaveCriticalSection(&m_Mutex);
#elif defined(OMEGA_POSIX)
    ::pthread_mutex_unlock(&m_Mutex);
#endif
}

//-------------------------------------------------------------------------------------------

template <class X> BSemaphoreItem *BSemaphoreQueue<X>::getItem()
{
    RSemaphoreItem *item;

    item = new RSemaphoreItem;

#if defined(OMEGA_WIN32)
    item->unit.cond = ::CreateEvent(0,FALSE,FALSE,0);
    ::InitializeCriticalSection(&item->unit.mutex);
#elif defined(OMEGA_POSIX)
    ::pthread_cond_init(&item->unit.cond,0);
    ::pthread_mutex_init(&item->unit.mutex,0);
#endif
    item->item = 0;
    return reinterpret_cast<BSemaphoreItem *>(item);
}

//-------------------------------------------------------------------------------------------

template <class X> void BSemaphoreQueue<X>::freeItem(BSemaphoreItem *item)
{
    if(item!=0)
    {
        RSemaphoreItem *rItem = reinterpret_cast<RSemaphoreItem *>(item);

#if defined(OMEGA_WIN32)
        ::DeleteCriticalSection(&rItem->unit.mutex);
        ::CloseHandle(rItem->unit.cond);
#elif defined(OMEGA_POSIX)
        ::pthread_mutex_destroy(&rItem->unit.mutex);
        ::pthread_cond_destroy(&rItem->unit.cond);
#endif
        delete item;
    }
}

//-------------------------------------------------------------------------------------------

template <class X> void BSemaphoreQueue<X>::lockItem(BSemaphoreItem *item)
{
    if(item!=0)
    {
#if defined(OMEGA_WIN32)
        ::EnterCriticalSection(&item->mutex);
#elif defined(OMEGA_POSIX)
        ::pthread_mutex_lock(&item->mutex);
#endif
    }
}

//-------------------------------------------------------------------------------------------

template <class X> void BSemaphoreQueue<X>::unlockItem(BSemaphoreItem *item)
{
    if(item!=0)
    {
#if defined(OMEGA_WIN32)
        ::LeaveCriticalSection(&item->mutex);
#elif defined(OMEGA_POSIX)
        ::pthread_mutex_unlock(&item->mutex);
#endif    
    }
}

//-------------------------------------------------------------------------------------------

template <class X> void BSemaphoreQueue<X>::Up(X *x)
{
    if(x!=0)
    {
        RSemaphoreItem *item;
        
        lock();
        if(m_List.Size()>0)
        {
            item = reinterpret_cast<RSemaphoreItem *>(m_List.Find(0));
            m_List.Delete(0);
            item->item = x;
            
            lockItem(reinterpret_cast<BSemaphoreItem *>(item));
            unlockItem(reinterpret_cast<BSemaphoreItem *>(item));
            
#if defined(OMEGA_WIN32)
            ::SetEvent(item->unit.cond);
#elif defined(OMEGA_POSIX)
            ::pthread_cond_signal(&item->unit.cond);
#endif
        }
        else
        {
            tint pos;

            pos = m_Queue.Size();
            m_Queue.Add(pos,x);
        }
        unlock();
    }
}

//-------------------------------------------------------------------------------------------

template <class X> X *BSemaphoreQueue<X>::Down()
{
    tint pos;
    X *x = 0;
    RSemaphoreItem *item;
    
    lock();
    if(m_Queue.Size()<=0)
    {
        item = reinterpret_cast<RSemaphoreItem *>(getItem());
        
        lockItem(reinterpret_cast<BSemaphoreItem *>(item));
        pos = m_List.Size();
        m_List.Add(pos,reinterpret_cast<BSemaphoreItem *>(item));
        unlock();
        
#if defined(OMEGA_WIN32)
        unlockItem(reinterpret_cast<BSemaphoreItem *>(item));
        if(::WaitForSingleObject(item->unit.cond,static_cast<DWORD>(m_Timeout))==WAIT_TIMEOUT)
#elif defined(OMEGA_POSIX)
        tint r;
        struct timespec ts;
        struct timeval tp;
        struct timezone tz;
        long timeout;
        
        ::gettimeofday(&tp,&tz);
        
        timeout = static_cast<long>(m_Timeout * 1000) + tp.tv_usec;
        tp.tv_sec += static_cast<time_t>(timeout / 1000000);
        tp.tv_usec = timeout % 1000000;
        ts.tv_sec = tp.tv_sec;
        ts.tv_nsec = tp.tv_usec * 1000;
        
        r = ::pthread_cond_timedwait(&item->unit.cond,&item->unit.mutex,&ts);
        unlockItem(reinterpret_cast<BSemaphoreItem *>(item));
        if(r==ETIMEDOUT)
#endif
        {
            tint i;
            BSemaphoreItem *delItem;
            
            lock();
            if(item->item==0)
            {
                for(i=0;i<m_List.Size();++i)
                {
                    delItem = m_List.Find(i);
                    if(delItem==reinterpret_cast<BSemaphoreItem *>(item))
                    {
                        m_List.Delete(i);
                        break;
                    }
                }
            }
            else
            {
                x = item->item;
            }
            freeItem(reinterpret_cast<BSemaphoreItem *>(item));
            unlock();
        }
        else
        {
            lock();
            x = item->item;
            freeItem(reinterpret_cast<BSemaphoreItem *>(item));
            unlock();
        }
    }
    else
    {
        x = m_Queue.Find(0);
        m_Queue.Delete(0);    
        unlock();
    }
    return x;
}

//-------------------------------------------------------------------------------------------

template <class X> tint BSemaphoreQueue<X>::Count() const
{
    tint c;
    
    lock();
    c = m_Queue.Size();
    unlock();
    return c;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
