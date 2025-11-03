#include "common/inc/Mutex.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

Mutex::Mutex()
{
#if defined(OMEGA_WIN32) || defined(OMEGA_WIN64)
    ::InitializeCriticalSection(&m_Mutex);
#elif defined(OMEGA_POSIX)
    ::pthread_mutex_init(&m_Mutex,0);
#endif
}

//-------------------------------------------------------------------------------------------

Mutex::~Mutex()
{
#if defined(OMEGA_WIN32) || defined(OMEGA_WIN64)
    ::DeleteCriticalSection(&m_Mutex);
#elif defined(OMEGA_POSIX)
    ::pthread_mutex_destroy(&m_Mutex);
#endif
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
