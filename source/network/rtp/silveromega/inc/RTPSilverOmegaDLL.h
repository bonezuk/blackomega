//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_SILVEROMEGA_RTPSILVEROMEGADLL_H
#define __OMEGA_NETWORK_RTP_SILVEROMEGA_RTPSILVEROMEGADLL_H
//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)
#if defined(OMEGA_NETWORK_RTPSILVEROMEGA_DLL)
#define RTPSILVEROMEGA_EXPORT __declspec(dllexport)
#define RTPSILVEROMEGA_TEMPLATE
#else
#define RTPSILVEROMEGA_EXPORT __declspec(dllimport)
#define RTPSILVEROMEGA_TEMPLATE extern
#endif
#elif defined(OMEGA_POSIX)
#define RTPSILVEROMEGA_EXPORT
#define RTPSILVEROMEGA_TEMPLATE
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
    namespace network
    {
        namespace rtp
        {
            namespace silveromega
            {
            } // namespace silveromega
        } // namespace rtp
    } // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
