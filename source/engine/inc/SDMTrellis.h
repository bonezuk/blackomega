//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SDMTRELLIS_H
#define __OMEGA_ENGINE_SDMTRELLIS_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/FIRFilterDB.h"

#include <cstdint>
#include <vector>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

template <typename T> struct SDMTrellisFilter
{
    T *a;
    T *g;
    int rate;
};

using SDMTrellisFilter_Float = SDMTrellisFilter<float>;
using SDMTrellisFilter_Double = SDMTrellisFilter<double>;

//-------------------------------------------------------------------------------------------

template <typename T> struct SDMTrellisState
{
    T *state;
    T cost;
};

using SDMTrellisState_Float = SDMTrellisState<float>;
using SDMTrellisState_Double = SDMTrellisState<double>;

//-------------------------------------------------------------------------------------------

template <typename T> ENGINE_EXPORT SDMTrellisFilter<T> *getSDMTrellisFilter(int dsdRate, bool isClans);
template <typename T> ENGINE_EXPORT void freeSDMFreeTrellisFilter(SDMTrellisFilter<T> *filter);
template <typename T> ENGINE_EXPORT SDMTrellisState<T> *allocateSMDTrellisStateArray(int size);
template <typename T> ENGINE_EXPORT void freeSDMFreeTrellisStateArray(SDMTrellisState<T> *states, int size);

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT bool sdmCalcTrellisFilter4Lanes(const SDMTrellisState_Float *src, SDMTrellisState_Float *dest, const SDMTrellisFilter_Float *filter, float x);
ENGINE_EXPORT bool sdmCalcTrellisFilter4Lanes(const SDMTrellisState_Double *src, SDMTrellisState_Double *dest, const SDMTrellisFilter_Double *filter, double x);

ENGINE_EXPORT bool sdmCalcTrellisFilter8Lanes(const SDMTrellisState_Float *src, SDMTrellisState_Float *dest, const SDMTrellisFilter_Float *filter, float x);
ENGINE_EXPORT bool sdmCalcTrellisFilter8Lanes(const SDMTrellisState_Double *src, SDMTrellisState_Double *dest, const SDMTrellisFilter_Double *filter, double x);

//-------------------------------------------------------------------------------------------
// Highway target selection for the SIMD trellis calculations. Highway is linked statically
// into the engine so the chosen target must be set from within the engine and not the caller.
//-------------------------------------------------------------------------------------------

// Targets (HWY_AVX2, HWY_SSE4, ...) that are both compiled in and supported by the CPU.
ENGINE_EXPORT std::vector<int64_t> sdmTrellisSupportedTargets();
ENGINE_EXPORT const char *sdmTrellisTargetName(int64_t target);
// Force dispatch to the given target. Passing 0 restores automatic selection of the best target.
ENGINE_EXPORT void sdmTrellisSetTarget(int64_t target);

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
