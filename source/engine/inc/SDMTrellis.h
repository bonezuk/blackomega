//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SDMTRELLIS_H
#define __OMEGA_ENGINE_SDMTRELLIS_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/FIRFilterDB.h"

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

template <typename T> SDMTrellisFilter<T> *getSDMTrellisFilter(int dsdRate, bool isClans);
template <typename T> void freeSDMFreeTrellisFilter(SDMTrellisState<T> *filter);
template <typename T> SDMTrellisState<T> *allocateSMDTrellisStateArray(int size);
template <typename T> void freeSDMFreeTrellisStateArray(SDMTrellisState<T> *states, int size);

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT bool sdmCalcTrellisFilter4Lanes(const SDMTrellisState_Float *src, SDMTrellisState_Float *dest, const SDMTrellisFilter_Float *filter, float x);
ENGINE_EXPORT bool sdmCalcTrellisFilter4Lanes(const SDMTrellisState_Double *src, SDMTrellisState_Double *dest, const SDMTrellisFilter_Double *filter, double x);

ENGINE_EXPORT bool sdmCalcTrellisFilter8Lanes(const SDMTrellisState_Float *src, SDMTrellisState_Float *dest, const SDMTrellisFilter_Float *filter, float x);
ENGINE_EXPORT bool sdmCalcTrellisFilter8Lanes(const SDMTrellisState_Double *src, SDMTrellisState_Double *dest, const SDMTrellisFilter_Double *filter, double x);

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
