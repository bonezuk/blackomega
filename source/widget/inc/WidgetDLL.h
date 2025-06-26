//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_WIDGET_WIDGETDLL_H
#define __OMEGA_WIDGET_WIDGETDLL_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)
#if defined(OMEGA_WIDGET_DLL)
#define WIDGET_EXPORT __declspec(dllexport)
#define WIDGET_TEMPLATE
#else
#define WIDGET_EXPORT __declspec(dllimport)
#define WIDGET_TEMPLATE extern
#endif
#else
#define WIDGET_EXPORT
#define WIDGET_TEMPLATE extern
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
    namespace widget
    {
    } // namespace widget
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
