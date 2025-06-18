//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_GREENOMEGA_WHITEOMEGADLL_H
#define __OMEGA_ENGINE_GREENOMEGA_WHITEOMEGADLL_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)
#if defined(OMEGA_ENGINE_GREENOMEGA_DLL)
#define GREENOMEGA_EXPORT __declspec(dllexport)
#define GREENOMEGA_TEMPLATE
#else
#define GREENOMEGA_EXPORT __declspec(dllimport)
#define GREENOMEGA_TEMPLATE extern
#endif
#elif defined(OMEGA_POSIX)
#define GREENOMEGA_EXPORT
#define GREENOMEGA_TEMPLATE
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
    namespace engine
    {
        namespace greenomega
        {
        } // namespace greenomega
    } // namespace engine
}// namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
