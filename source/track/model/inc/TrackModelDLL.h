//-------------------------------------------------------------------------------------------
#ifndef __ORUCS_TRACK_MODEL_TRACKMODELDLL_H
#define __ORUCS_TRACK_MODEL_TRACKMODELDLL_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)
#if defined(OMEGA_TRACK_MODEL_DLL)
#define TRACK_MODEL_EXPORT __declspec(dllexport)
#define TRACK_MODEL_TEMPLATE
#else
#define TRACK_MODEL_EXPORT __declspec(dllimport)
#define TRACK_MODEL_TEMPLATE extern
#endif
#else
#define TRACK_MODEL_EXPORT
#define TRACK_MODEL_TEMPLATE extern
#endif

//-------------------------------------------------------------------------------------------

namespace omega
{
    namespace track
    {
        namespace model
        {
        } // namespace model
    } // namespace track
} // namespace omega

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
