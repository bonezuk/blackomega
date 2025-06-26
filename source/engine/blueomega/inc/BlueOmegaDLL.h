//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BLUEOMEGA_BLUEOMEGADLL_H
#define __OMEGA_ENGINE_BLUEOMEGA_BLUEOMEGADLL_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)
#if defined(OMEGA_ENGINE_BLUEOMEGA_DLL)
#define BLUEOMEGA_EXPORT __declspec(dllexport)
#define BLUEOMEGA_TEMPLATE
#else
#define BLUEOMEGA_EXPORT __declspec(dllimport)
#define BLUEOMEGA_TEMPLATE extern
#endif
#else
#define BLUEOMEGA_EXPORT
#define BLUEOMEGA_TEMPLATE extern
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
    namespace engine
    {
        namespace blueomega
        {
        } // namespace blueomega
    } // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
