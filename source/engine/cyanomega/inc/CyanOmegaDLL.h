//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_CYANOMEGA_CYANOMEGADLL_H
#define __OMEGA_ENGINE_CYANOMEGA_CYANOMEGADLL_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)
#if defined(OMEGA_ENGINE_CYANOMEGA_DLL)
#define CYANOMEGA_EXPORT __declspec(dllexport)
#define CYANOMEGA_TEMPLATE
#else
#define CYANOMEGA_EXPORT __declspec(dllimport)
#define CYANOMEGA_TEMPLATE extern
#endif
#elif defined(OMEGA_POSIX)
#define CYANOMEGA_EXPORT
#define CYANOMEGA_TEMPLATE
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
    namespace engine
    {
        namespace cyanomega
        {
        } // namespace cyanomega
    } // namespace engine
}// namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
