//-------------------------------------------------------------------------------------------
#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "engine/src/SDMTrellisSIMD.cxx"
#include "hwy/foreach_target.h"
#include "hwy/highway.h"

//-------------------------------------------------------------------------------------------

template <typename T> struct SDMTrellisFilter
{
    T *a;
    T *g;
    int rate;
    const char *name;
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

typedef struct {
    const double a[8];
    const double g[8];
    int rate;
    const char *name;
} SDMTrellisFilterBase;

#define SDM_TRELLIS_NO_OF_FILTERS 6

static constexpr SDMTrellisFilterBase c_sdmTrellisFilters[SDM_TRELLIS_NO_OF_FILTERS] = {
    {
        { 
            1.15188624720851e+00, 5.45054196257555e-01, 1.38703640845632e-01, 2.07076444822072e-02,
            1.85506614417771e-03, 9.63403135615390e-05, 2.69174565706992e-06, 2.22594461751768e-08,
        },
        {
            5.06749566262594e-06, 0.0, 4.15924517416912e-05, 0.0,
            9.55783346944871e-05, 0.0, 1.38868728742641e-04, 0.0,
        },
        256,
        "clans-8",
    }, {
        {
            7.42329617949054e-01, 2.72509195471757e-01, 6.41424039739473e-02, 1.05299412132258e-02,
            1.23178223428228e-03, 9.94985029720342e-05, 5.13169547054423e-06, 1.20466411041020e-07,
        },
        {
            5.06749566262594e-06, 0.0, 4.15924517416912e-05, 0.0,
            9.55783346944871e-05, 0.0, 1.38868728742641e-04, 0.0,
        },
        256,
        "sdm-8",
    }, {
        {
            1.04472698053970e+00, 4.62088167600438e-01, 1.13484722685479e-01, 1.68939738398161e-02,
            1.55891676875336e-03, 8.23864822188133e-05, 2.39690238375972e-06, -1.75063180618551e-09,
        },
        {
            2.02698799324546e-05, 0.0, 1.66362887238597e-04, 0.0,
            3.82276797905696e-04, 0.0, 5.55397776875272e-04, 0.0,
        },
        128,
        "clans-8",
    }, {
        {
            7.42763211426562e-01, 2.71983157679393e-01, 6.36389361390464e-02, 1.03289230528372e-02,
            1.19045645863092e-03, 9.25357160397986e-05, 4.64982367004083e-06, 8.14280266547840e-08,
        },
        {
            2.02698799324546e-05, 0.0, 1.66362887238597e-04, 0.0,
            3.82276797905696e-04, 0.0, 5.55397776875272e-04, 0.0,
        },
        128,
        "sdm-8",
    }, {
        {
            1.18730059129261e+00, 5.66733317291325e-01, 1.40117339676942e-01, 1.87599862200771e-02,
            1.27685506908071e-03, 8.76397405988154e-06, -1.90294986721073e-06, -7.39020160622772e-08,
        },
        {
            8.10778762576884e-05, 0.0, 6.65340842513387e-04, 0.0,
            1.52852264942192e-03, 0.0, 2.22035724073886e-03, 0.0,
        },
        64,
        "clans-8",
    }, {
        {
            7.44453769826547e-01, 2.69850507860307e-01, 6.16093616071757e-02, 9.52771711245796e-03,
            1.02903114196526e-03, 6.63758229311911e-05, 2.91124056073927e-06, -4.29323230577427e-08,
        },
        {
            8.10778762576884e-05, 0.0, 6.65340842513387e-04, 0.0,
            1.52852264942192e-03, 0.0, 2.22035724073886e-03, 0.0,
        },
        64,
        "sdm-8",
    }
};

//-------------------------------------------------------------------------------------------
HWY_BEFORE_NAMESPACE();
namespace omega
{
namespace engine
{
namespace HWY_NAMESPACE
{

namespace hw = hwy::HWY_NAMESPACE;
//-------------------------------------------------------------------------------------------

template <typename T> void sdmCalcTrellisFilter_F4Lane(SDMTrellisState_Float *src, SDMTrellisState_Float *dest, SDMTrellisFilter_Float* filter, float x)
{
    const hn::FixedTag<T, 4> d;

    auto h0 = hn::LoadU(d, src->state);              // (s[0], s[1], s[2], s[3])
    auto h1 = hn::LoadU(d, src->state + 4);          // (s[4], s[5], s[6], s[7])
    auto f0 = hn::Slide1Up(d, h0);                      // (   0, s[0], s[1], s[2])
    f0 = hn::InsertLane(f0, 0, x);                      // (   x, s[0], s[1], s[2])
    auto f1 = hn::CombineShiftRightLanes<3>(d, h1, h0); // (s[3], s[4], s[5], s[6])
    auto k0 = hn::CombineShiftRightLanes<1>(d, h1, h0); // (s[1], s[2], s[3], s[4])
    auto k1 = hn::ShiftRightLanes<1>(d, h1);            // (s[5], s[6], s[7],  0.0)
    
    // f0 = (   x, s[0], s[1], s[2])
    // g0 = (g[0], g[1], g[2], g[3])
    // k0 = (s[1], s[2], s[3], s[4])
    // f0 = f0 - g0 * k0
    // f0 = ( x - g[0] * s[1], s[0] - g[1] * s[2], s[1] - g[2] * s[3], s[2] - g[3] * s[4])
    f0 = hn::NegMulAdd(hn::LoadU(d, filter->g), k0, f0);

    // h0 = (s[0], s[1], s[2], s[3])
    // f0 = (   x + s[0] - g[0] * s[1], = d[0]
    //       s[0] + s[1] - g[1] * s[2], = d[1]
    //       s[1] + s[2] - g[2] * s[3], = d[2]
    //       s[2] + s[3] - g[3] * s[4]) = d[3]
    f0 = hn::Add(f0, h0);

    // f1 = (s[3], s[4], s[5], s[6])
    // g1 = (g[4], g[5], g[6],  0.0)
    // k1 = (s[5], s[6], s[7],  0.0)
    // f1 = f1 - g1 * k0
    // f1 = (s[3] - g[4] * s[5], s[4] - g[5] * s[6], s[5] - g[6] * s[7], s[6])
    f1 = hn::NegMulAdd(hn::LoadU(d, filter->g + 4), k1, f1);
    hn::StoreU(f0, d, dest[0].state);
    hn::StoreU(f0, d, dest[1].state);

    // h1 = (s[4], s[5], s[6], s[7])
    // f1 = (s[3] + s[4] - g[4] * s[5], = d[4]
    //       s[4] + s[5] - g[5] * s[6], = d[5]
    //       s[5] + s[6] - g[6] * s[7], = d[6]
    //       s[6] + s[7])               = d[7]
    f1 = hn::Add(f1, h1);
    hn::StoreU(f1, d, dest[0].state + 4);
    hn::StoreU(f1, d, dest[1].state + 4);

    auto v = hn::Dup128VecFromValues(d, x, 0.0f, 0.0f, 0.0f);   // (x[0], 0.0, 0.0, 0.0)
    // (x + a[0]*d[0], a[1]*d[1], a[2]*d[2], a[3]*d[3])
    v = hn::MulAdd(hn::LoadU(d, filter->a), f0, v);
    // (x + a[0]*d[0] + a[4]*d[4], a[1]*d[1] + a[5]d[5], a[2]*d[2] + a[6]d[6], a[3]*d[3] + a[7]d[7])
    v = hn::MulAdd(hn::LoadU(d, filter->a + 4, f1, v));

    T vSum = hn::ReduceSum(d, v);
    
    dest[0].state[0] += static_cast<T>(1.0);
    dest[1].state[0] -= static_cast<T>(1.0);

    T cost = src->cost;
    T v0 = v + filter->a[0];
    T v1 = v - filter->a[0];
    dest[0].cost = cost + (v0 * v0);
    dest[1].cost = cost + (v1 * v1);
}


//-------------------------------------------------------------------------------------------
} // namespace HWY_NAMESPACE
} // namespace engine
} // namespace omega
HWY_AFTER_NAMESPACE();
//-------------------------------------------------------------------------------------------
#if HWY_ONE
//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

template <typename T> void freeSDMFreeTrellisFilter(SDMTrellisState<T> *filter)
{
    if(filter != nullptr)
    {
        if(filter->a != nullptr)
        {
            hwy::FreeAlignedBytes(filter->a, nullptr, nullptr);
        }
        if(filter->g != nullptr)
        {
            hwy::FreeAlignedBytes(filter->g, nullptr, nullptr);
        }
        free(filter);
    }
}

template <typename T> SDMTrellisFilter<T> *getSDMTrellisFilter(int dsdRate, bool isClans)
{
    const SDMTrellisFilterBase *bFilter = nullptr;
    SDMTrellisFilter<T> *filter = nullptr;

    for(int idx = 0; idx < SDM_TRELLIS_NO_OF_FILTERS && bFilter == nullptr; idx++)
    {
        if(dsdRate == c_sdmTrellisFilters[idx].rate)
        {
            if(isClans && strncmp("clans", c_sdmTrellisFilters[idx].name, 5) == 0)
            {
                bFilter = &c_sdmTrellisFilters[idx];
            }
            else if(!isClans && strncmp("sdm", c_sdmTrellisFilters[idx].name, 3) == 0)
            {
                bFilter = &c_sdmTrellisFilters[idx];
            }
        }
    }
    if(bFilter != nullptr)
    {
        filter = calloc(1, sizeof(SDMTrellisFilter<T>));
        if(filter != nullptr)
        {
            filter->a = hwy::AllocateAligned<T>(8).release();
            filter->g = hwy::AllocateAligned<T>(8).release();
            if(filter->a != nullptr && filter->g != nullptr)
            {
                memcpy(filter->a, bFilter->a, 8 * sizeof(T));
                memcpy(filter->g, bFilter->g, 8 * sizeof(T));
                filter->rate = bFilter->rate;
                filter->name = bFilter->name;
            }
            else
            {
                freeSDMFreeTrellisFilter<T>(filter);
                filter = nullptr;
            }
        }
    }
    return filter;
}

template <typename T> void freeSDMFreeTrellisStateArray(SDMTrellisState<T> *states, int size)
{
    if(states != nullptr)
    {
        for(int idx = 0; idx < size; idx++)
        {
            if(states[idx].state != nullptr)
            {
                hwy::FreeAlignedBytes(states[idx].state, nullptr, nullptr);
                states[idx].state = nullptr;
            }
        }
        free(states);
    }
}

template <typename T> SDMTrellisState<T> *allocateSMDTrellisStateArray_Float(int size)
{
    SDMTrellisState_Float *states = calloc(size, sizeof(SDMTrellisState<T>));
    if(states != nullptr)
    {
        for(int idx = 0; idx < size; idx++)
        {
            states[idx].state = hwy::AllocateAligned<T>(8).release();
            if(states[idx].state == nullptr)
            {
                freeSDMFreeTrellisStateArray<T>(states, size);
                return nullptr;
            }
        }
    }
    return states;
}


//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
