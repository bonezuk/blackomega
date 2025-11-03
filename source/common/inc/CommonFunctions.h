//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_COMMONFUNCTIONS_H
#define __OMEGA_COMMON_COMMONFUNCTIONS_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"
#include "common/inc/BString.h"

#include <QString>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

COMMON_EXPORT QString getOSErrorString(tint errCode);

COMMON_EXPORT void msleepThread(tint msecs);
COMMON_EXPORT void usleepThread(tint usecs);

COMMON_EXPORT void loadSharedLibrary(const char *libName);

COMMON_EXPORT tuint64 elfHash64(tuint8 *mem, int len);
COMMON_EXPORT tuint64 elfHash64(tuint8 *mem, int len, tuint64 hash);

//-------------------------------------------------------------------------------------------

inline void xorSwap(int& x, int& y)
{
    x ^= y;
    y ^= x;
    x ^= y;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
