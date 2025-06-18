#ifndef __OMEGA_COMMON_COMMONDLL_H
#define __OMEGA_COMMON_COMMONDLL_H

#if defined(OMEGA_WIN32)

#define OMEGA_EXPORT_TEMPLATE extern
#define OMEGA_TEMPLATE_MEMBER

#if defined(OMEGA_COMMON_DLL)
#define COMMON_EXPORT __declspec(dllexport)
#define COMMON_TEMPLATE
#else
#define COMMON_EXPORT __declspec(dllimport)
#define COMMON_TEMPLATE extern
#endif

#elif defined(OMEGA_POSIX)

#define COMMON_EXPORT
#define COMMON_TEMPLATE
#define OMEGA_EXPORT_TEMPLATE extern
#define OMEGA_TEMPLATE_MEMBER template<>

#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
    namespace common
    {
    } // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

#endif
