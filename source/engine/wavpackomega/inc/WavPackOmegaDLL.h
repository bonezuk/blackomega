//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_WAVPACKOMEGA_WAVPACKOMEGADLL_H
#define __OMEGA_ENGINE_WAVPACKOMEGA_WAVPACKOMEGADLL_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)
#if defined(OMEGA_ENGINE_WAVPACKOMEGA_DLL)
#define WAVPACKOMEGA_EXPORT __declspec(dllexport)
#define WAVPACKOMEGA_TEMPLATE
#else
#define WAVPACKOMEGA_EXPORT __declspec(dllimport)
#define WAVPACKOMEGA_TEMPLATE extern
#endif
#elif defined(OMEGA_POSIX)
#define WAVPACKOMEGA_EXPORT
#define WAVPACKOMEGA_TEMPLATE
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
    namespace engine
    {
        namespace wavpackomega
        {
        } // namespace wavpackomega
    } // namespace engine
}// namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
