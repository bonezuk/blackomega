//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_REMOTEDLL_H
#define __OMEGA_REMOTE_REMOTEDLL_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)
#if defined(OMEGA_REMOTE_DLL)
#define REMOTE_EXPORT __declspec(dllexport)
#define REMOTE_TEMPLATE
#else
#define REMOTE_EXPORT __declspec(dllimport)
#define REMOTE_TEMPLATE extern
#endif
#else
#define REMOTE_EXPORT
#define REMOTE_TEMPLATE extern
#endif

//-------------------------------------------------------------------------------------------

namespace omega
{
    namespace remote
    {
    } // namespace remote
} // namespace omega

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
