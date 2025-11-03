#ifndef __OMEGA_COMMON_SEMAPHORE_H
#define __OMEGA_COMMON_SEMAPHORE_H

#include "common/inc/CommonTypes.h"
#include "common/inc/BOQueueTree.h"

#if defined(OMEGA_POSIX)
#include <errno.h>
#include <sys/time.h>
#endif

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
} SemaphoreItem;
//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------
typedef struct
{
    pthread_cond_t cond;
    pthread_mutex_t mutex;
} SemaphoreItem;
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

#if !defined(COMP_UNIT_Semaphore)
OMEGA_EXPORT_TEMPLATE template class COMMON_EXPORT BOQueueTree<SemaphoreItem *>;
#elif defined(OMEGA_WIN32)
template class COMMON_EXPORT BOQueueTree<SemaphoreItem *>;
#endif

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT Semaphore
{
    public:
        Semaphore();
        Semaphore(tint n);
        virtual ~Semaphore();
        
        virtual void Up();
        
        virtual bool Down();
        virtual bool Down(tint timeout);
        
        virtual tint Get() const;

#if defined(OMEGA_WIN32)    
        CRITICAL_SECTION m_Mutex;
#elif defined(OMEGA_POSIX)
        pthread_mutex_t m_Mutex;
#endif

    protected:
        
        BOQueueTree<SemaphoreItem *> m_List;
        BOQueueTree<SemaphoreItem *> m_Free;
        
        tint m_Count;
        
        virtual SemaphoreItem *getItem();
        virtual void freeItem(SemaphoreItem *item);
        
        virtual void lock();
        virtual void lock(SemaphoreItem *item);
        
        virtual void unlock();
        virtual void unlock(SemaphoreItem *item);
};

//-------------------------------------------------------------------------------------------

inline tint Semaphore::Get() const
{
    return m_Count;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

#endif

