//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_VIOLETOMEGA_VIOLETOMEGADLL_H
#define __OMEGA_ENGINE_VIOLETOMEGA_VIOLETOMEGADLL_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)
#if defined(OMEGA_ENGINE_VIOLETOMEGA_DLL)
#define VIOLETOMEGA_EXPORT __declspec(dllexport)
#define VIOLETOMEGA_TEMPLATE
#else
#define VIOLETOMEGA_EXPORT __declspec(dllimport)
#define VIOLETOMEGA_TEMPLATE extern
#endif
#elif defined(OMEGA_POSIX)
#define VIOLETOMEGA_EXPORT
#define VIOLETOMEGA_TEMPLATE
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
    namespace engine
    {
        namespace violetomega
        {
        } // namespace violetomega
    } // namespace engine
}// namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
