//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_FIRFILTERDB_H
#define __OMEGA_ENGINE_FIRFILTERDB_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/EngineDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

typedef enum 
{
    e_NoFilter = 0,
    e_lowPassHalf_8192 = 1,
    e_lowPassHalf_4097 = 2,
    e_lowPassQuarter_8193 = 3,
    e_lpHalf_DSD0_5, // 1025
    e_lpHalf_DSD1, // 2049
    e_lpHalf_DSD2, // 4097 = e_lowPassHalf_4097
    e_lpQuarter_DSD2,
    e_lpHalf_DSD4, // 8193
    e_lpQuarter_DSD4, // = e_lowPassQuarter_8193
    e_lpHalf_DSD8, // 16385
    e_lpQuarter_DSD8,
    e_lpQuarter_DSD16, // 32769
    e_lpQuarter_DSD32, // 65537
    e_lpQuarter_DSD64, // 131073
    e_lpQuarter_DSD128, // 262145
    e_lpQuarter_DSD256, // 524289
    e_lpQuarter_DSD512, // 1048577
    e_lpQuarter_DSD1024, // 2097153
} FIRFilterType;

ENGINE_EXPORT double *getFIRFilterFromDB(FIRFilterType filterType, int& len);

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

