#ifndef __OMEGA_COMMON_COMMONTYPES_H
#define __OMEGA_COMMON_COMMONTYPES_H

//-------------------------------------------------------------------------------------------
// OMEGA_WIN32  - Windows 32-bit development
// OMEGA_MACOSX - Mac OS X development
// OMEGA_LINUX  - Linux development
//
// OMEGA_WINDOWS - Windows platform
// OMEGA_POSIX   - Posix standard API
//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)
#define OMEGA_WINDOWS
#define OMEGA_MSVC
#define OMEGA_DEFINE_TEMPLATE_HEADER
#else
#define OMEGA_POSIX
#endif

#define OMEGA_LITTLE_ENDIAN

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WINDOWS)
#include <windows.h>
#elif defined(OMEGA_POSIX)
#include <pthread.h>
#endif

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)
#define COMMON_EXPORT_TEMPLATE extern
#define COMMON_TEMPLATE_MEMBER
#else
#define COMMON_EXPORT_TEMPLATE extern
#if defined(OMEGA_POSIX)
#define COMMON_TEMPLATE_MEMBER template<>
#else
#define COMMON_TEMPLATE_MEMBER
#endif
#endif

//-------------------------------------------------------------------------------------------

#include "common/inc/CommonDLL.h"

#include <math.h>
#include <stdlib.h>
#include <stdint.h>

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

typedef char tchar;
typedef char tbyte;
typedef char tint8;

typedef unsigned char tuchar;
typedef unsigned char tubyte;
typedef unsigned char tuint8;

typedef short tshort;
typedef short tint16;

typedef unsigned short tushort;
typedef unsigned short tuint16;

typedef int tint;
typedef int32_t tint32;

typedef unsigned int tuint;
typedef uint32_t tuint32;

typedef float tfloat32;
typedef double tfloat64;

typedef __int64 tint64;
typedef unsigned __int64 tuint64;

typedef HANDLE t_thread;

const tchar c_Separator[] = "\\";
const tchar c_SeparatorChar = '\\';

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MACOSX)
//-------------------------------------------------------------------------------------------

typedef char tchar;
typedef char tbyte;
typedef char tint8;

typedef unsigned char tuchar;
typedef unsigned char tubyte;
typedef unsigned char tuint8;

typedef short tshort;
typedef short tint16;

typedef unsigned short tushort;
typedef unsigned short tuint16;

typedef int tint;
typedef int32_t tint32;

typedef unsigned int tuint;
typedef uint32_t tuint32;

typedef float tfloat32;
typedef double tfloat64;

typedef long long tint64;
typedef unsigned long long tuint64;

typedef pthread_t t_thread;

const tchar c_Separator[] = "/";
const tchar c_SeparatorChar = '/';

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------

typedef char tchar;
typedef char tbyte;
typedef char tint8;

typedef unsigned char tuchar;
typedef unsigned char tubyte;
typedef unsigned char tuint8;

typedef short tshort;
typedef short tint16;

typedef unsigned short tushort;
typedef unsigned short tuint16;

typedef int tint;
typedef int32_t tint32;

typedef unsigned int tuint;
typedef uint32_t tuint32;

typedef float tfloat32;
typedef double tfloat64;

typedef long long tint64;
typedef unsigned long long tuint64;

typedef pthread_t t_thread;

const tchar c_Separator[] = "/";
const tchar c_SeparatorChar = '/';

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_IOS) || defined(OMEGA_ANDROID)
//-------------------------------------------------------------------------------------------

typedef char tchar;
typedef char tbyte;
typedef char tint8;

typedef unsigned char tuchar;
typedef unsigned char tubyte;
typedef unsigned char tuint8;

typedef int16_t tshort;
typedef int16_t tint16;

typedef uint16_t tushort;
typedef uint16_t tuint16;

typedef int tint;
typedef int32_t tint32;

typedef unsigned int tuint;
typedef uint32_t tuint32;

typedef float tfloat32;
typedef double tfloat64;

typedef int64_t tint64;
typedef uint64_t tuint64;

typedef pthread_t t_thread;

const tchar c_Separator[] = "/";
const tchar c_SeparatorChar = '/';

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

const tfloat64 c_PI_D = 3.141592653589793238464264338832795;
const tfloat32 c_PI_F = 3.141592653589793238464264338832795f;

const tfloat64 c_SQR_TWO_D = 1.414213562373095;
const tfloat32 c_SQR_TWO_F = 1.414213562373095f;

const tfloat64 c_SQR_HALF_D = 0.707106781186548;
const tfloat32 c_SQR_HALF_F = 0.707106781186548f;

//-------------------------------------------------------------------------------------------
// Define sample format type
//-------------------------------------------------------------------------------------------
#if defined(SINGLE_FLOAT_SAMPLE)
typedef tfloat32 sample_t;
const sample_t c_zeroSample = 0.0f;
const sample_t c_plusOneSample = 1.0f;
const sample_t c_negOneSample = -1.0f;
const sample_t c_halfSample = 0.5f;
#else
typedef tfloat64 sample_t;
const sample_t c_zeroSample = 0.0;
const sample_t c_plusOneSample = 1.0;
const sample_t c_negOneSample = -1.0;
const sample_t c_halfSample = 0.5;
#endif
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_DOUBLE)
//-------------------------------------------------------------------------------------------
const tfloat64 c_PI = c_PI_D;
typedef tfloat64 real;
//-------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------
const tfloat32 c_PI = c_PI_F;
typedef tfloat32 real;
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

#include "common/inc/BString.h"

//-------------------------------------------------------------------------------------------

#if defined (OMEGA_MSVC)
#define COMMON_DB(x,y,z) { omega::common::BString tmpErrString; tmpErrString << x << "::" << y << " - " << z << "\r\n"; ::OutputDebugStringA(static_cast<const char *>(tmpErrString)); }
#else
#define COMMON_DB(x,y,z)
#endif

//-------------------------------------------------------------------------------------------

inline bool isEqual(tfloat32 a,tfloat32 b,double maxRelativeError = 0.00001,double maxAbsoluteError = 1.0e-6)
{
    if(::fabs(a - b) < maxAbsoluteError)
    {
        return true;
    }
    else
    {
        double relativeError;
        
        if(::fabs(b) > ::fabs(a))
        {
            relativeError = ::fabs((a - b) / b);
        }
        else
        {
            relativeError = ::fabs((a - b) / a);
        }
        return (relativeError <= maxRelativeError) ? true : false;
    }    
}

//-------------------------------------------------------------------------------------------

inline bool isEqual(tfloat64 a,tfloat64 b,double maxRelativeError = 0.00001,double maxAbsoluteError = 1.0e-6)
{
    if(::fabs(a - b) < maxAbsoluteError)
    {
        return true;
    }
    else
    {
        double relativeError;
        
        if(::fabs(b) > ::fabs(a))
        {
            relativeError = ::fabs((a - b) / b);
        }
        else
        {
            relativeError = ::fabs((a - b) / a);
        }
        return (relativeError <= maxRelativeError) ? true : false;
    }    
}

//-------------------------------------------------------------------------------------------

inline tfloat32 toRadians(tfloat32 angleDegree)
{
    const tfloat32 c_angleToRadians = c_PI_F / 180.0f;
    return angleDegree * c_angleToRadians;
}


//-------------------------------------------------------------------------------------------

inline tfloat32 toDegrees(tfloat32 angleRadian)
{
    const tfloat32 c_radiansToAngle = 180.0f / c_PI_F;
    return angleRadian * c_radiansToAngle;
}

//-------------------------------------------------------------------------------------------

#endif
