//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_DSD_DSDOMEGADLL_H
#define __OMEGA_ENGINE_DSD_DSDOMEGADLL_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)
#if defined(OMEGA_ENGINE_DSDOMEGA_DLL)
#define DSDOMEGA_EXPORT __declspec(dllexport)
#define DSDOMEGA_TEMPLATE
#else
#define DSDOMEGA_EXPORT __declspec(dllimport)
#define DSDOMEGA_TEMPLATE extern
#endif
#else
#define DSDOMEGA_EXPORT
#define DSDOMEGA_TEMPLATE extern
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
	namespace engine
	{
		namespace dsd
		{
		} // namespace dsd
	} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
