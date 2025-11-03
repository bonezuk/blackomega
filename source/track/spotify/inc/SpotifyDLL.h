//-------------------------------------------------------------------------------------------
#ifndef __ORUCS_TRACK_SPOTIFY_TRACKSPOTIFYDLL_H
#define __OMEGA_TRACK_SPOTIFY_TRACKSPOTIFYDLL_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)
#if defined(OMEGA_TRACK_SPOTIFY_DLL)
#define TRACK_SPOTIFY_EXPORT __declspec(dllexport)
#define TRACK_SPOTIFY_TEMPLATE
#else
#define TRACK_SPOTIFY_EXPORT __declspec(dllimport)
#define TRACK_SPOTIFY_TEMPLATE extern
#endif
#else
#define TRACK_SPOTIFY_EXPORT
#define TRACK_SPOTIFY_TEMPLATE extern
#endif

//-------------------------------------------------------------------------------------------

namespace omega
{
    namespace track
    {
        namespace spotify
        {
        } // namespace spotify
    } // namespace track
} // namespace omega

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
