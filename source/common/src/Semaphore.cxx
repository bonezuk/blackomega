#if !defined(COMP_UNIT_Semaphore)
#define COMP_UNIT_Semaphore
#endif

#include "common/inc/Semaphore.h"
#include "common/inc/BSemaphoreQueue.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

COMMON_TEMPLATE_MEMBER BSemaphoreItem *BOQueueTree<BSemaphoreItem *>::nullObject = 0;

COMMON_TEMPLATE_MEMBER SemaphoreItem *BOQueueTree<SemaphoreItem *>::nullObject = 0;

//-------------------------------------------------------------------------------------------


Semaphore::Semaphore() : m_List(),
    m_Free(),
    m_Count(0)
{
#if defined(OMEGA_WIN32)
    ::InitializeCriticalSection(&m_Mutex);
#elif defined(OMEGA_POSIX)
    ::pthread_mutex_init(&m_Mutex,0);
#endif
}

//-------------------------------------------------------------------------------------------

Semaphore::Semaphore(tint n) : m_List(),
    m_Free(),
    m_Count(n)
{
#if defined(OMEGA_WIN32)
    ::InitializeCriticalSection(&m_Mutex);
#elif defined(OMEGA_POSIX)
    ::pthread_mutex_init(&m_Mutex,0);
#endif
}

//-------------------------------------------------------------------------------------------

Semaphore::~Semaphore()
{
    try
    {
        SemaphoreItem *item;

        Semaphore::lock();
        while(item=m_List.Find(0),item!=0)
        {
            m_List.Delete(0); //lint !e534

            Semaphore::lock(item);
            Semaphore::unlock(item);

#if defined(OMEGA_WIN32)
            ::SetEvent(item->cond); //lint !e534
#elif defined(OMEGA_POSIX)
            ::pthread_cond_signal(&item->cond);
#endif
            Semaphore::freeItem(item);
        }
        Semaphore::unlock();

        Semaphore::lock();
        while(item=m_Free.Find(0),item!=0)
        {
            m_Free.Delete(0); //lint !e534
#if defined(OMEGA_WIN32)
            ::DeleteCriticalSection(&item->mutex);
            ::CloseHandle(item->cond); //lint !e534
#elif defined(OMEGA_POSIX)
            ::pthread_mutex_destroy(&item->mutex);
            ::pthread_cond_destroy(&item->cond);
#endif
            delete item;
        }
        Semaphore::unlock();

#if defined(OMEGA_WIN32)
        ::DeleteCriticalSection(&m_Mutex);
#elif defined(OMEGA_POSIX)
        ::pthread_mutex_destroy(&m_Mutex);
#endif
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

SemaphoreItem *Semaphore::getItem()
{
    SemaphoreItem *item;

    if(m_Free.Size()>0)
    {
        item = m_Free.Find(0);
        m_Free.Delete(0); //lint !e534
    }
    else
    {
        item = new SemaphoreItem;
#if defined(OMEGA_WIN32)
        item->cond = ::CreateEvent(0,FALSE,FALSE,0);
        ::InitializeCriticalSection(&item->mutex);
#elif defined(OMEGA_POSIX)
        ::pthread_cond_init(&item->cond,0);
        ::pthread_mutex_init(&item->mutex,0);
#endif
    }
    return item;
}

//-------------------------------------------------------------------------------------------

void Semaphore::freeItem(SemaphoreItem *item)
{
    if(item!=0)
    {
        tint pos;

        pos = m_Free.Size();
        m_Free.Add(pos,item); //lint !e534
    }
}

//-------------------------------------------------------------------------------------------

void Semaphore::lock()
{
#if defined(OMEGA_WIN32)
    ::EnterCriticalSection(&m_Mutex);
#elif defined(OMEGA_POSIX)
    ::pthread_mutex_lock(&m_Mutex);
#endif
}

//-------------------------------------------------------------------------------------------

void Semaphore::lock(SemaphoreItem *item)
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

void Semaphore::unlock()
{
#if defined(OMEGA_WIN32)
    ::LeaveCriticalSection(&m_Mutex);
#elif defined(OMEGA_POSIX)
    ::pthread_mutex_unlock(&m_Mutex);
#endif
}

//-------------------------------------------------------------------------------------------

void Semaphore::unlock(SemaphoreItem *item)
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

void Semaphore::Up()
{
    lock();
    m_Count++;
    if(m_List.Size()>0)
    {
        SemaphoreItem *item;

        item = m_List.Find(0);
        m_List.Delete(0); //lint !e534

        lock(item);
        unlock(item);

#if defined _NET_OS_WINDOWS
        ::SetEvent(item->cond); //lint !e534
#elif defined _NET_OS_UNIX
        ::pthread_cond_signal(&item->cond);
#endif

        freeItem(item);
    }
    unlock();
}

//-------------------------------------------------------------------------------------------

bool Semaphore::Down()
{
    tint pos;
    SemaphoreItem *item;

    lock();
    if(m_Count<=0)
    {
        item = getItem();

        lock(item);

        pos = m_List.Size();
        m_List.Add(pos,item); //lint !e534
        m_Count--;

        unlock();

#if defined(OMEGA_WIN32)
        unlock(item);
        ::WaitForSingleObject(item->cond,INFINITE); //lint !e534
#elif defined(OMEGA_POSIX)
        ::pthread_cond_wait(&item->cond,&item->mutex);
        unlock(item);
#endif
    }
    else
    {
        m_Count--;
        unlock();
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool Semaphore::Down(tint timeout)
{
    tint pos;
    SemaphoreItem *item;
    bool res = true;

    lock();
    if(m_Count<=0)
    {
        item = getItem();

        lock(item);

        pos = m_List.Size();
        m_List.Add(pos,item); //lint !e534
        m_Count--;

        unlock();

#if defined(OMEGA_WIN32)
        unlock(item);
        if(::WaitForSingleObject(item->cond,static_cast<DWORD>(timeout))==WAIT_TIMEOUT)
#elif defined(OMEGA_POSIX)
        tint r;
        struct timeval now;
        struct timespec t;
        struct timezone tz;

        ::gettimeofday(&now,&tz);
        now.tv_usec += (timeout * 1000) % 1000000;
        if(now.tv_usec>=1000000)
        {
            now.tv_usec %= 1000000;
            now.tv_sec++;
        }
        now.tv_sec += timeout / 1000;

        t.tv_sec = now.tv_sec;
        t.tv_nsec = now.tv_usec * 1000;

        r = ::pthread_cond_timedwait(&item->cond,&item->mutex,&t);
        unlock(item);
        if(r==ETIMEDOUT)
#endif
        {
            tint i;
            SemaphoreItem *delItem;

            lock();

            for(i=0;i<m_List.Size();++i)
            {
                delItem = m_List.Find(i);
                if(delItem==item)
                {
                    m_List.Delete(i); //lint !e534
                    freeItem(item);
                    m_Count++;
                    break;
                }
            }
            res = false;
            unlock();
        }
    }
    else
    {
        m_Count--;
        unlock();
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
