//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_ENGINEDLL_H
#define __OMEGA_ENGINE_ENGINEDLL_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)
#if defined(OMEGA_ENGINE_DLL)
#define ENGINE_EXPORT __declspec(dllexport)
#define ENGINE_TEMPLATE
#else
#define ENGINE_EXPORT __declspec(dllimport)
#define ENGINE_TEMPLATE extern
#endif
#else
#define ENGINE_EXPORT
#define ENGINE_TEMPLATE extern
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
    namespace engine
    {
    } // namespace engine
} // namespace omega

//-------------------------------------------------------------------------------------------

#endif
//-------------------------------------------------------------------------------------------
