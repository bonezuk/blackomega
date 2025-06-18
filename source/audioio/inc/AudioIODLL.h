//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AUDIOIODLL_H
#define __OMEGA_AUDIOIO_AUDIOIODLL_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)
#if defined(OMEGA_AUDIOIO_DLL)
#define AUDIOIO_EXPORT __declspec(dllexport)
#define AUDIOIO_TEMPLATE
#else
#define AUDIOIO_EXPORT __declspec(dllimport)
#define AUDIOIO_TEMPLATE extern
#endif
#else
#define AUDIOIO_EXPORT
#define AUDIOIO_TEMPLATE extern
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
    namespace audioio
    {
    } // namespace engine
} // namespace omega

//-------------------------------------------------------------------------------------------

#endif
//-------------------------------------------------------------------------------------------
