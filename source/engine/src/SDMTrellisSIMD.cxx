#include <cstdlib>
#include <cstring>

#include "hwy/aligned_allocator.h"

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "engine/src/SDMTrellisSIMD.cxx"
#include "hwy/foreach_target.h"
#include "hwy/highway.h"

#include "engine/inc/SDMTrellis.h"

//-------------------------------------------------------------------------------------------
HWY_BEFORE_NAMESPACE();
namespace omega
{
namespace engine
{
namespace HWY_NAMESPACE
{

namespace hn = hwy::HWY_NAMESPACE;
//-------------------------------------------------------------------------------------------

template <typename T> void sdmCalcTrellisFilter_4Lanes(const SDMTrellisState<T> *src, SDMTrellisState<T> *dest, const SDMTrellisFilter<T> *filter, T x)
{
    const hn::FixedTag<T, 4> d;
    using V = hn::Vec<decltype(d)>;

    // h0 = (s[0], s[1], s[2], s[3])
    const V h0 = hn::Load(d, src->state);
    // h1 = (s[4], s[5], s[6], s[7])
    const V h1 = hn::Load(d, src->state + 4);

    // f0 = (   x, s[0], s[1], s[2])
    V f0 = hn::InsertLane(hn::Slide1Up(d, h0), 0, x);
    // f1 = (s[3], s[4], s[5], s[6])
    V f1 = hn::LoadU(d, src->state + 3);
    // k0 = (s[1], s[2], s[3], s[4])
    V k0 = hn::LoadU(d, src->state + 1);
    // k1 = (s[5], s[6], s[7],  0.0)
    const V k1 = hn::Slide1Down(d, h1);

    // f0 = (   x, s[0], s[1], s[2])
    // g0 = (g[0], g[1], g[2], g[3])
    // k0 = (s[1], s[2], s[3], s[4])
    // f0 = f0 - g0 * k0
    // f0 = ( x - g[0] * s[1], s[0] - g[1] * s[2], s[1] - g[2] * s[3], s[2] - g[3] * s[4])
    f0 = hn::NegMulAdd(hn::Load(d, filter->g), k0, f0);

    // h0 = (s[0], s[1], s[2], s[3])
    // f0 = (   x + s[0] - g[0] * s[1], = d[0]
    //       s[0] + s[1] - g[1] * s[2], = d[1]
    //       s[1] + s[2] - g[2] * s[3], = d[2]
    //       s[2] + s[3] - g[3] * s[4]) = d[3]
    f0 = hn::Add(f0, h0);

    hn::Store(f0, d, dest[0].state);
    hn::Store(f0, d, dest[1].state);

    // f1 = (s[3], s[4], s[5], s[6])
    // g1 = (g[4], g[5], g[6],  0.0)
    // k1 = (s[5], s[6], s[7],  0.0)
    // f1 = f1 - g1 * k0
    // f1 = (s[3] - g[4] * s[5], s[4] - g[5] * s[6], s[5] - g[6] * s[7], s[6])
    f1 = hn::NegMulAdd(hn::Load(d, filter->g + 4), k1, f1);

    // h1 = (s[4], s[5], s[6], s[7])
    // f1 = (s[3] + s[4] - g[4] * s[5], = d[4]
    //       s[4] + s[5] - g[5] * s[6], = d[5]
    //       s[5] + s[6] - g[6] * s[7], = d[6]
    //       s[6] + s[7])               = d[7]
    f1 = hn::Add(f1, h1);

    hn::Store(f1, d, dest[0].state + 4);
    hn::Store(f1, d, dest[1].state + 4);

    // v = (x[0], 0.0, 0.0, 0.0)
    V v = hn::InsertLane(hn::Zero(d), 0, x);
    // v = (x + a[0]*d[0], a[1]*d[1], a[2]*d[2], a[3]*d[3])
    v = hn::MulAdd(hn::Load(d, filter->a), f0, v);
    // v = (x + a[0]*d[0] + a[4]*d[4], a[1]*d[1] + a[5]d[5], a[2]*d[2] + a[6]d[6], a[3]*d[3] + a[7]d[7])
    v = hn::MulAdd(hn::Load(d, filter->a + 4), f1, v);

    T vSum = hn::ReduceSum(d, v);
    
    dest[0].state[0] += static_cast<T>(1.0);
    dest[1].state[0] -= static_cast<T>(1.0);

    T cost = src->cost;
    T v0 = vSum + filter->a[0];
    T v1 = vSum - filter->a[0];
    dest[0].cost = cost + (v0 * v0);
    dest[1].cost = cost + (v1 * v1);
}

//-------------------------------------------------------------------------------------------

template <typename T> void sdmCalcTrellisFilter_8Lanes(const SDMTrellisState<T> *src, SDMTrellisState<T> *dest, const SDMTrellisFilter<T> *filter, T x)
{
    const hn::FixedTag<T, 8> d;
    using V = hn::Vec<decltype(d)>;

    // h = (s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7])
    const V h = hn::Load(d, src->state);
    // f = (   x, s[0], s[1], s[2], s[3], s[4], s[5], s[6])
    V f = hn::InsertLane(hn::Slide1Up(d, h), 0, x);
    // k = (s[1], s[2], s[3], s[4], s[5], s[6], s[7],  0.0)
    V k = hn::Slide1Down(d, h);

    // f = (   x, s[0], s[1], s[2], s[3], s[4], s[5], s[6])
    // k = (s[1], s[2], s[3], s[4], s[5], s[6], s[7],  0.0)
    // f = (   x - g[0] * s[1], 
    //      s[0] - g[1] * s[2], 
    //      s[1] - g[2] * s[3], 
    //      s[2] - g[3] * s[4], 
    //      s[3] - g[4] * s[5], 
    //      s[4] - g[5] * s[6], 
    //      s[5] - g[6] * s[7], 
    //      s[6] - g[7] *  0.0)
    f = hn::NegMulAdd(hn::Load(d, filter->g), k, f);

    // f = (   x + s[0] - g[0] * s[1], = d[0]
    //      s[0] + s[1] - g[1] * s[2], = d[1]
    //      s[1] + s[2] - g[2] * s[3], = d[2]
    //      s[2] + s[3] - g[3] * s[4], = d[3]
    //      s[3] + s[4] - g[4] * s[5], = d[4]
    //      s[4] + s[5] - g[5] * s[6], = d[5]
    //      s[5] + s[6] - g[6] * s[7], = d[6]
    //      s[6] + s[7] - g[7] *  0.0) = d[7]
    f = hn::Add(f, h);

    hn::Store(f, d, dest[0].state);
    hn::Store(f, d, dest[1].state);

    // v = (   x,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0)
    V v = hn::InsertLane(hn::Zero(d), 0, x);
    // v = ( x + a[0]*d[0], a[1]*d[1], a[2]*d[2], a[3]*d[3], a[4]*d[4], a[5]*d[5], a[6]*d[6], a[7]*d[7])
    v = hn::MulAdd(hn::Load(d, filter->a), f, v);

    const T vSum = hn::ReduceSum(d, v);

    dest[0].state[0] += static_cast<T>(1.0);
    dest[1].state[0] -= static_cast<T>(1.0);

    const T cost = src->cost;
    const T v0 = vSum + filter->a[0];
    const T v1 = vSum - filter->a[0];
    dest[0].cost = cost + (v0 * v0);
    dest[1].cost = cost + (v1 * v1);
}

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisFilter_4Lanes_Float(const SDMTrellisState_Float *src, SDMTrellisState_Float *dest, const SDMTrellisFilter_Float *filter, float x)
{
#if HWY_MAX_BYTES >= 16
    sdmCalcTrellisFilter_4Lanes<float>(src, dest, filter, x);
    return true;
#else
    (void)src; (void)dest; (void)filter; (void)x;
    return false;
#endif
}

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisFilter_8Lanes_Float(const SDMTrellisState_Float *src, SDMTrellisState_Float *dest, const SDMTrellisFilter_Float *filter, float x)
{
#if !HWY_HAVE_SCALABLE && HWY_MAX_BYTES >= 32
    sdmCalcTrellisFilter_8Lanes<float>(src, dest, filter, x);
    return true;
#else
    (void)src; (void)dest; (void)filter; (void)x;
    return false;
#endif
}

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisFilter_4Lanes_Double(const SDMTrellisState_Double *src, SDMTrellisState_Double *dest, const SDMTrellisFilter_Double *filter, double x)
{
#if HWY_HAVE_FLOAT64 && !HWY_HAVE_SCALABLE && HWY_MAX_BYTES >= 32
    sdmCalcTrellisFilter_4Lanes<double>(src, dest, filter, x);
    return true;
#else
    (void)src; (void)dest; (void)filter; (void)x;
    return false;
#endif
}

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisFilter_8Lanes_Double(const SDMTrellisState_Double *src, SDMTrellisState_Double *dest, const SDMTrellisFilter_Double *filter, double x)
{
#if HWY_HAVE_FLOAT64 && !HWY_HAVE_SCALABLE && HWY_MAX_BYTES >= 64
    sdmCalcTrellisFilter_8Lanes<double>(src, dest, filter, x);
    return true;
#else
    (void)src; (void)dest; (void)filter; (void)x;
    return false;
#endif
}

//-------------------------------------------------------------------------------------------
// Rearrange to use state[j][path]
//-------------------------------------------------------------------------------------------

template <typename T, typename D> void sdmCalcTrellisFilterBlock(D d, const SDMTrellisStates<T> *src, 
    SDMTrellisStates<T> *dest, const SDMTrellisBlockFilter<T> *filter, T x, int fromPathIndex)
{
    using V = hn::Vec<decltype(d)>;
    constexpr size_t N = hn::MaxLanes(d);
    int toPathIndexA = fromPathIndex << 1;
    int toPathIndexB = toPathIndexA + static_cast<int>(N);

    // s0 = (s0[0], s1[0], s2[0], s3[0])
    const V s0 = hn::Load(d, &src->states[0][fromPathIndex]);
    // d0 = (    x,     x,     x,     x)
    V d0 = hn::Set(d, x);
    // d0 = (x+s0[0], x+s1[0], x+s2[0], x+s3[0])
    d0 = hn::Add(s0, d0);
    // s1 = (s0[1], s1[1], s2[1], s3[1])
    const V s1 = hn::Load(d, &src->states[1][fromPathIndex]);
    // d0 = (x+s0[0]-g[0]*s0[1], x+s1[0]-g[0]*s1[1], x+s2[0]-g[0]*s2[1], x+s3[0]-g[0]*s3[1]) = (d0[0], d1[0], d2[0], d3[0])
    d0 = hn::NegMulAdd(hn::Load(d, &filter->g[0][0]), s1, d0);

    // a0 = (a[0], a1[0], a2[0], a3[0])
    const V a0 = hn::Load(d, &filter->a[0][0]);
    // v = (x+a[0]*d0[0], x+a[0]*d1[0], x+a[0]*d2[0], x+a[0]*d3[0])
    V v = hn::MulAdd(a0, d0, hn::Set(d, x));

    // d1 = (s0[1]+s0[1], s1[1]+s1[1], s2[1]+s2[1], s3[1]+s3[1]) = (d0[1], d1[1], d2[1], d3[1])
    V d1 = hn::Add(s0, s1);
    hn::Store(d1, d, &dest->states[1][toPathIndexA]);
    hn::Store(d1, d, &dest->states[1][toPathIndexB]);
    
    // v += (a[1]*d0[1], a[1]*d1[1], a[1]*d2[1], a[1]*d3[1])
    v = hn::MulAdd(hn::Load(d, &filter->a[1][0]), d1, v);

    // s2 = (s0[2], s1[2], s2[2], s3[2])
    const V s2 = hn::Load(d, &src->states[2][fromPathIndex]);
    // d2 = (s0[1]+s0[2], s1[1]+s1[2], s2[1]+s2[2], s3[1]+s3[2])
    V d2 = hn::Add(s1, s2);
    // s3 = (s0[3], s1[3], s2[3], s3[3])
    const V s3 = hn::Load(d, &src->states[3][fromPathIndex]);
    // d2 = (s0[1]+s0[2]-g[2]*s0[3], s1[1]+s1[2]-g[2]*s1[3], s2[1]+s2[2]-g[2]*s2[3], s3[1]+s3[2]-g[2]*s3[3]) = (d0[2], d1[2], d2[2], d3[2])
    d2 = hn::NegMulAdd(hn::Load(d, &filter->g[1][0]), s3, d2);
    hn::Store(d2, d, &dest->states[2][toPathIndexA]);
    hn::Store(d2, d, &dest->states[2][toPathIndexB]);

    // v += (a[2]*d0[2], a[2]*d1[2], a[2]*d2[2], a[2]*d3[2])
    v = hn::MulAdd(hn::Load(d, &filter->a[2][0]), d2, v);

    // d3 = (s0[2]+s0[3], s1[2]+s1[3], s2[2]+s2[3], s3[2]+s3[3]) = (d0[3], d1[3], d2[3], d3[3])
    V d3 = hn::Add(s2, s3);
    hn::Store(d3, d, &dest->states[3][toPathIndexA]);
    hn::Store(d3, d, &dest->states[3][toPathIndexB]);

    // v += (a[3]*d0[3], a[3]*d1[3], a[3]*d2[3], a[3]*d3[3])
    v = hn::MulAdd(hn::Load(d, &filter->a[3][0]), d3, v);

    // s4 = (s0[4], s1[4], s2[4], s3[4])
    const V s4 = hn::Load(d, &src->states[4][fromPathIndex]);
    // d4 = (s0[3]+s0[4], s1[3]+s1[4], s2[3]+s2[4], s3[3]+s3[4])
    V d4 = hn::Add(s3, s4);
    // s5 = (s0[5], s1[5], s2[5], s3[5])
    const V s5 = hn::Load(d, &src->states[5][fromPathIndex]);
    // d4 = (s0[3]+s0[4]-g[4]*s0[5], s1[3]+s1[4]-g[4]*s1[5], s2[3]+s2[4]-g[4]*s2[5], s3[3]+s3[4]-g[4]*s3[5]) = (d0[4], d1[4], d2[4], d3[4])
    d4 = hn::NegMulAdd(hn::Load(d, &filter->g[2][0]), s5, d4);
    hn::Store(d4, d, &dest->states[4][toPathIndexA]);
    hn::Store(d4, d, &dest->states[4][toPathIndexB]);

    // v += (a[4]*d0[4], a[4]*d1[4], a[4]*d2[4], a[4]*d3[4])
    v = hn::MulAdd(hn::Load(d, &filter->a[4][0]), d4, v);

    // d5 = (s0[4]+s0[5], s1[4]+s1[5], s2[4]+s2[5], s3[4]+s3[5]) = (d0[5], d1[5], d2[5], d3[5])
    V d5 = hn::Add(s4, s5);
    hn::Store(d5, d, &dest->states[5][toPathIndexA]);
    hn::Store(d5, d, &dest->states[5][toPathIndexB]);

    // v += (a[5]*d0[5], a[5]*d1[5], a[5]*d2[5], a[5]*d3[5])
    v = hn::MulAdd(hn::Load(d, &filter->a[5][0]), d5, v);

    // s6 = (s0[6], s1[6], s2[6], s3[6])
    const V s6 = hn::Load(d, &src->states[6][fromPathIndex]);
    // d6 = (s0[5]+s0[6], s1[5]+s1[6], s2[5]+s2[6], s3[5]+s3[6])
    V d6 = hn::Add(s5, s6);
    // s7 = (s0[7], s1[7], s2[7], s3[7])
    const V s7 = hn::Load(d, &src->states[7][fromPathIndex]);
    // d6 = (s0[5]+s0[6]-g[6]*s0[7], s1[5]+s1[6]-g[6]*s1[7], s2[5]+s2[6]-g[6]*s2[7], s3[5]+s3[6]-g[6]*s3[7]) = (d0[6], d1[6], d2[6], d3[6])
    d6 = hn::NegMulAdd(hn::Load(d, &filter->g[3][0]), s7, d6);
    hn::Store(d6, d, &dest->states[6][toPathIndexA]);
    hn::Store(d6, d, &dest->states[6][toPathIndexB]);

    // v += (a[6]*d0[6], a[6]*d1[6], a[6]*d2[6], a[6]*d3[6])
    v = hn::MulAdd(hn::Load(d, &filter->a[6][0]), d6, v);

    // d7 = (s0[6]+s0[7], s1[6]+s1[7], s2[6]+s2[7], s3[6]+s3[7]) = (d0[7], d1[7], d2[7], d3[7])
    V d7 = hn::Add(s6, s7);
    hn::Store(d7, d, &dest->states[7][toPathIndexA]);
    hn::Store(d7, d, &dest->states[7][toPathIndexB]);

    // v += (a[7]*d0[7], a[7]*d1[7], a[7]*d2[7], a[7]*d3[7]) = ( v0, v1, v2, v3)
    v = hn::MulAdd(hn::Load(d, &filter->a[7][0]), d7, v);

    // one = ( 1.0,   1.0,    1.0,   1.0)
    const V one = hn::Set(d, T(1.0));
    // d0A = (d0[0]+1.0, d1[0]+1.0, d2[0]+1.0, d3[0]+1.0)
    const V d0A = hn::Add(d0, one);
    hn::Store(d0A, d, &dest->states[0][toPathIndexA]);
    // d0B = (d0[0]-1.0, d1[0]-1.0, d2[0]-1.0, d3[0]-1.0)
    const V d0B = hn::Sub(d0, one);
    hn::Store(d0B, d, &dest->states[0][toPathIndexB]);
    
    const V costSrc = hn::Load(d, &src->cost[fromPathIndex]);

    // vA = (v0+a[0], v1+a[0], v2+a[0], v3+a[0])
    const V vA = hn::Add(v, a0);
    // costA = (costA0+sqr(v0+a[0]), costA1+sqr(v1+a[0]), costA2+sqr(v2+a[0]), costA3+sqr(v3+a[0]))
    const V costA = hn::MulAdd(vA, vA, costSrc);
    hn::Store(costA, d, &dest->cost[toPathIndexA]);

    // vB = (v0-a[0], v1-a[0], v2-a[0], v3-a[0])
    const V vB = hn::Sub(v, a0);
    // costB = (costB0+sqr(v0-a[0]), costB1+sqr(v1-a[0]), costB2+sqr(v2-a[0]), costB3+sqr(v3-a[0]))
    const V costB = hn::MulAdd(vB, vB, costSrc);
    hn::Store(costB, d, &dest->cost[toPathIndexB]);
}

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisBlockFilter_4Lanes_Float(const SDMTrellisStates_Float *src, SDMTrellisStates_Float *dest, 
    const SDMTrellisBlockFilter_Float *filter, float x, int fromPathIndex)
{
#if HWY_MAX_BYTES >= 16
    const hn::FixedTag<float, 4> d;
    sdmCalcTrellisFilterBlock(d, src, dest, filter, x, fromPathIndex);
    return true;
#else
    (void)src; (void)dest; (void)filter; (void)x; (void)fromPathIndex;
    return false;
#endif
}

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisBlockFilter_8Lanes_Float(const SDMTrellisStates_Float *src, SDMTrellisStates_Float *dest, 
    const SDMTrellisBlockFilter_Float *filter, float x, int fromPathIndex)
{
#if !HWY_HAVE_SCALABLE && HWY_MAX_BYTES >= 32
    const hn::FixedTag<float, 8> d;
    sdmCalcTrellisFilterBlock(d, src, dest, filter, x, fromPathIndex);
    return true;
#else
    (void)src; (void)dest; (void)filter; (void)x; (void)fromPathIndex;
    return false;
#endif
}

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisBlockFilter_4Lanes_Double(const SDMTrellisStates_Double *src, SDMTrellisStates_Double *dest, 
    const SDMTrellisBlockFilter_Double *filter, double x, int fromPathIndex)
{
#if HWY_HAVE_FLOAT64 && !HWY_HAVE_SCALABLE && HWY_MAX_BYTES >= 32
    const hn::FixedTag<double, 4> d;
    sdmCalcTrellisFilterBlock(d, src, dest, filter, x, fromPathIndex);
    return true;
#else
    (void)src; (void)dest; (void)filter; (void)x; (void)fromPathIndex;
    return false;
#endif
}

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisBlockFilter_8Lanes_Double(const SDMTrellisStates_Double *src, SDMTrellisStates_Double *dest, 
    const SDMTrellisBlockFilter_Double *filter, double x, int fromPathIndex)
{
#if HWY_HAVE_FLOAT64 && !HWY_HAVE_SCALABLE && HWY_MAX_BYTES >= 64
    const hn::FixedTag<double, 8> d;
    sdmCalcTrellisFilterBlock(d, src, dest, filter, x, fromPathIndex);
    return true;
#else
    (void)src; (void)dest; (void)filter; (void)x; (void)fromPathIndex;
    return false;
#endif
}

//-------------------------------------------------------------------------------------------
} // namespace HWY_NAMESPACE
} // namespace engine
} // namespace omega
HWY_AFTER_NAMESPACE();
//-------------------------------------------------------------------------------------------
#if HWY_ONCE
//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

typedef struct {
    const double a[8];
    const double g[8];
    int rate;
    const char *name;
} SDMTrellisFilterBase;

#define SDM_TRELLIS_NO_OF_FILTERS 6

//-------------------------------------------------------------------------------------------

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

const SDMTrellisFilterBase *getTrellisBaseFilter(int dsdRate, bool isClans)
{
    const SDMTrellisFilterBase *bFilter = nullptr;

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
    return bFilter;
}

//-------------------------------------------------------------------------------------------

template <typename T> void freeSDMFreeTrellisBlockFilter(SDMTrellisBlockFilter<T> *filter)
{
    if(filter != nullptr)
    {
        delete filter;
    }
}

//-------------------------------------------------------------------------------------------

template <typename T> SDMTrellisBlockFilter<T> *getSDMTrellisBlockFilter(int dsdRate, bool isClans)
{
    const SDMTrellisFilterBase *bFilter = getTrellisBaseFilter(dsdRate, isClans);
    SDMTrellisBlockFilter<T> *filter;

    if(bFilter != nullptr)
    {
        filter = new SDMTrellisBlockFilter<T>();
        if(filter != nullptr)
        {
            for(int coeffIdx = 0; coeffIdx < 8; coeffIdx++)
            {
                for(int idx = 0; idx < 8; idx++)
                {
                    filter->a[coeffIdx][idx] = static_cast<T>(bFilter->a[coeffIdx]);
                }
            }
            for(int coeffIdx = 0; coeffIdx < 4; coeffIdx++)
            {
                for(int idx = 0; idx < 8; idx++)
                {
                    filter->g[coeffIdx][idx] = static_cast<T>(bFilter->g[coeffIdx << 1]);
                }
            }
        }
    }
    return filter;
}

//-------------------------------------------------------------------------------------------

template <typename T> void freeSDMFreeTrellisFilter(SDMTrellisFilter<T> *filter)
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

//-------------------------------------------------------------------------------------------

template <typename T> SDMTrellisFilter<T> *getSDMTrellisFilter(int dsdRate, bool isClans)
{
    const SDMTrellisFilterBase *bFilter = getTrellisBaseFilter(dsdRate, isClans);
    SDMTrellisFilter<T> *filter = nullptr;

    if(bFilter != nullptr)
    {
        filter = static_cast<SDMTrellisFilter<T> *>(calloc(1, sizeof(SDMTrellisFilter<T>)));
        if(filter != nullptr)
        {
            filter->a = hwy::AllocateAligned<T>(8).release();
            filter->g = hwy::AllocateAligned<T>(8).release();
            if(filter->a != nullptr && filter->g != nullptr)
            {
                for(int idx = 0; idx < 8; idx++)
                {
                    filter->a[idx] = static_cast<T>(bFilter->a[idx]);
                    filter->g[idx] = static_cast<T>(bFilter->g[idx]);
                }
                filter->rate = bFilter->rate;
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

//-------------------------------------------------------------------------------------------

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

//-------------------------------------------------------------------------------------------

template <typename T> SDMTrellisState<T> *allocateSMDTrellisStateArray(int size)
{
    SDMTrellisState<T> *states = static_cast<SDMTrellisState<T> *>(calloc(size, sizeof(SDMTrellisState<T>)));
    if(states != nullptr)
    {
        for(int idx = 0; idx < size; idx++)
        {
            states[idx].state = hwy::AllocateAligned<T>(8).release();
            if(states[idx].state != nullptr)
            {
                for(int j = 0; j < 8; j++)
                {
					states[idx].state[j] = static_cast<T>(0.0);
                }
            }
            else
            {
                freeSDMFreeTrellisStateArray<T>(states, size);
                return nullptr;
            }
        }
    }
    return states;
}

//-------------------------------------------------------------------------------------------

template <typename T> SDMTrellisStates<T> *allocateSMDTrellisStates()
{
    SDMTrellisStates<T> *states = new SDMTrellisStates<T>();
    if(states != nullptr)
    {
        for(int idx = 0; idx < c_maxNoSDMTrellisPaths; idx++)
        {
            for(int j = 0; j < 8; j++)
            {
                states->states[j][idx] = static_cast<T>(0.0);
            }
            states->cost[idx] = static_cast<T>(0.0);
        }
    }
    return states;
}

//-------------------------------------------------------------------------------------------

template <typename T> void freeSMDTrellisStates(SDMTrellisStates<T> *states)
{
    if(states != nullptr)
    {
        delete states;
    }
}

//-------------------------------------------------------------------------------------------
// Explicit instantiations
//-------------------------------------------------------------------------------------------

template ENGINE_EXPORT SDMTrellisFilter<float> *getSDMTrellisFilter<float>(int dsdRate, bool isClans);
template ENGINE_EXPORT void freeSDMFreeTrellisFilter<float>(SDMTrellisFilter<float> *filter);
template ENGINE_EXPORT SDMTrellisState<float> *allocateSMDTrellisStateArray<float>(int size);
template ENGINE_EXPORT void freeSDMFreeTrellisStateArray<float>(SDMTrellisState<float> *states, int size);

template ENGINE_EXPORT SDMTrellisFilter<double> *getSDMTrellisFilter<double>(int dsdRate, bool isClans);
template ENGINE_EXPORT void freeSDMFreeTrellisFilter<double>(SDMTrellisFilter<double> *filter);
template ENGINE_EXPORT SDMTrellisState<double> *allocateSMDTrellisStateArray<double>(int size);
template ENGINE_EXPORT void freeSDMFreeTrellisStateArray<double>(SDMTrellisState<double> *states, int size);

template ENGINE_EXPORT SDMTrellisBlockFilter<float> *getSDMTrellisBlockFilter(int dsdRate, bool isClans);
template ENGINE_EXPORT void freeSDMFreeTrellisBlockFilter(SDMTrellisBlockFilter<float> *filter);
template ENGINE_EXPORT SDMTrellisStates<float> *allocateSMDTrellisStates();
template ENGINE_EXPORT void freeSMDTrellisStates(SDMTrellisStates<float> *states);

template ENGINE_EXPORT SDMTrellisBlockFilter<double> *getSDMTrellisBlockFilter(int dsdRate, bool isClans);
template ENGINE_EXPORT void freeSDMFreeTrellisBlockFilter(SDMTrellisBlockFilter<double> *filter);
template ENGINE_EXPORT SDMTrellisStates<double> *allocateSMDTrellisStates();
template ENGINE_EXPORT void freeSMDTrellisStates(SDMTrellisStates<double> *states);

//-------------------------------------------------------------------------------------------

HWY_EXPORT(sdmCalcTrellisFilter_4Lanes_Float);
HWY_EXPORT(sdmCalcTrellisFilter_8Lanes_Float);
HWY_EXPORT(sdmCalcTrellisFilter_4Lanes_Double);
HWY_EXPORT(sdmCalcTrellisFilter_8Lanes_Double);

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisFilter4Lanes(const SDMTrellisState_Float *src, SDMTrellisState_Float *dest, const SDMTrellisFilter_Float *filter, float x)
{
    return HWY_DYNAMIC_DISPATCH(sdmCalcTrellisFilter_4Lanes_Float)(src, dest, filter, x);
}

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisFilter4Lanes(const SDMTrellisState_Double *src, SDMTrellisState_Double *dest, const SDMTrellisFilter_Double *filter, double x)
{
    return HWY_DYNAMIC_DISPATCH(sdmCalcTrellisFilter_4Lanes_Double)(src, dest, filter, x);
}

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisFilter8Lanes(const SDMTrellisState_Float *src, SDMTrellisState_Float *dest, const SDMTrellisFilter_Float *filter, float x)
{
    return HWY_DYNAMIC_DISPATCH(sdmCalcTrellisFilter_8Lanes_Float)(src, dest, filter, x);
}

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisFilter8Lanes(const SDMTrellisState_Double *src, SDMTrellisState_Double *dest, const SDMTrellisFilter_Double *filter, double x)
{
    return HWY_DYNAMIC_DISPATCH(sdmCalcTrellisFilter_8Lanes_Double)(src, dest, filter, x);
}

//-------------------------------------------------------------------------------------------

HWY_EXPORT(sdmCalcTrellisBlockFilter_4Lanes_Float);
HWY_EXPORT(sdmCalcTrellisBlockFilter_8Lanes_Float);
HWY_EXPORT(sdmCalcTrellisBlockFilter_4Lanes_Double);
HWY_EXPORT(sdmCalcTrellisBlockFilter_8Lanes_Double);

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisBlockFilter4Lanes(const SDMTrellisStates_Float *src, SDMTrellisStates_Float *dest, 
    const SDMTrellisBlockFilter_Float *filter, float x, int fromPathIndex)
{
    return HWY_DYNAMIC_DISPATCH(sdmCalcTrellisBlockFilter_4Lanes_Float)(src, dest, filter, x, fromPathIndex);
}

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisBlockFilter8Lanes(const SDMTrellisStates_Float *src, SDMTrellisStates_Float *dest, 
    const SDMTrellisBlockFilter_Float *filter, float x, int fromPathIndex)
{
    return HWY_DYNAMIC_DISPATCH(sdmCalcTrellisBlockFilter_8Lanes_Float)(src, dest, filter, x, fromPathIndex);
}

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisBlockFilter4Lanes(const SDMTrellisStates_Double *src, SDMTrellisStates_Double *dest, 
    const SDMTrellisBlockFilter_Double *filter, double x, int fromPathIndex)
{
    return HWY_DYNAMIC_DISPATCH(sdmCalcTrellisBlockFilter_4Lanes_Double)(src, dest, filter, x, fromPathIndex);
}

//-------------------------------------------------------------------------------------------

bool sdmCalcTrellisBlockFilter8Lanes(const SDMTrellisStates_Double *src, SDMTrellisStates_Double *dest, 
    const SDMTrellisBlockFilter_Double *filter, double x, int fromPathIndex)
{
    return HWY_DYNAMIC_DISPATCH(sdmCalcTrellisBlockFilter_8Lanes_Double)(src, dest, filter, x, fromPathIndex);
}

//-------------------------------------------------------------------------------------------

std::vector<int64_t> sdmTrellisSupportedTargets()
{
    return hwy::SupportedAndGeneratedTargets();
}

//-------------------------------------------------------------------------------------------

const char *sdmTrellisTargetName(int64_t target)
{
    return hwy::TargetName(target);
}

//-------------------------------------------------------------------------------------------

void sdmTrellisSetTarget(int64_t target)
{
    hwy::SetSupportedTargetsForTest(target);
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
