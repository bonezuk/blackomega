#ifndef __OMEGA_COMMON_MUTEX_H
#define __OMEGA_COMMON_MUTEX_H

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT Mutex
{
    public:
        Mutex();
        virtual ~Mutex();
        
        void lock();
        void unlock();
        
    protected:
    
#if defined(OMEGA_WIN32) || defined(OMEGA_WIN64)
        CRITICAL_SECTION m_Mutex;
#elif defined(OMEGA_POSIX)
        pthread_mutex_t m_Mutex;
#endif
};

//-------------------------------------------------------------------------------------------

inline void Mutex::lock()
{
#if defined(OMEGA_WIN32) || defined(OMEGA_WIN64)
    ::EnterCriticalSection(&m_Mutex);
#elif defined(OMEGA_POSIX)
    ::pthread_mutex_lock(&m_Mutex);
#endif
}

//-------------------------------------------------------------------------------------------

inline void Mutex::unlock()
{
#if defined(OMEGA_WIN32) || defined(OMEGA_WIN64)
    ::LeaveCriticalSection(&m_Mutex);
#elif defined(OMEGA_POSIX)
    ::pthread_mutex_unlock(&m_Mutex);
#endif
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

#endif
