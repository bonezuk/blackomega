//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SDMTRELLISPLAIN_H
#define __OMEGA_ENGINE_SDMTRELLISPLAIN_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/FIRFilterDB.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

template<typename X> class SDMTrellisPlain
{
    public:
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

#include "engine/inc/SDMTrellisPlain.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "engine/inc/SDMTrellisPlain.h"

//-------------------------------------------------------------------------------------------

class SDMTrellisSoxOriginalTester
{
    public:
       typedef struct sdm_filter {
            const double  a[MAX_FILTER_ORDER];
            const double  g[MAX_FILTER_ORDER];
            int32_t       order;
            unsigned      freq;
            const char   *name;
            int           trellis_order;
            int           trellis_num;
            int           trellis_lat;
        } sdm_filter_t;

        typedef struct sdm_state {
            double        state[MAX_FILTER_ORDER];
            double        cost;
            uint32_t      path;
            uint8_t       next;
            uint8_t       hist;
            uint8_t       hist_used;
            struct sdm_state *parent;
            struct sdm_state *path_list;
        } sdm_state_t;

        static constexpr sdm_filter_t sdm_filters[] = {
        { // 8
            {
                1.15188624720851e+00,
                5.45054196257555e-01,
                1.38703640845632e-01,
                2.07076444822072e-02,
                1.85506614417771e-03,
                9.63403135615390e-05,
                2.69174565706992e-06,
                2.22594461751768e-08,
            },
            {
                5.06749566262594e-06, 0,
                4.15924517416912e-05, 0,
                9.55783346944871e-05, 0,
                1.38868728742641e-04, 0,
            },
            8,
            256 * 44100,
            "clans-8",
            0, 0, 0
        },
        { // 9
            {
                7.42329617949054e-01,
                2.72509195471757e-01,
                6.41424039739473e-02,
                1.05299412132258e-02,
                1.23178223428228e-03,
                9.94985029720342e-05,
                5.13169547054423e-06,
                1.20466411041020e-07,
            },
            {
                5.06749566262594e-06, 0,
                4.15924517416912e-05, 0,
                9.55783346944871e-05, 0,
                1.38868728742641e-04, 0,
            },
            8,
            256 * 44100,
            "sdm-8",
            0, 0, 0
        },
        { // 18
            {
                1.04472698053970e+00,
                4.62088167600438e-01,
                1.13484722685479e-01,
                1.68939738398161e-02,
                1.55891676875336e-03,
                8.23864822188133e-05,
                2.39690238375972e-06,
                -1.75063180618551e-09,
            },
            {
                2.02698799324546e-05, 0,
                1.66362887238597e-04, 0,
                3.82276797905696e-04, 0,
                5.55397776875272e-04, 0,
            },
            8,
            128 * 44100,
            "clans-8",
            0, 0, 0
        },
        { // 19
            {
                7.42763211426562e-01,
                2.71983157679393e-01,
                6.36389361390464e-02,
                1.03289230528372e-02,
                1.19045645863092e-03,
                9.25357160397986e-05,
                4.64982367004083e-06,
                8.14280266547840e-08,
            },
            {
                2.02698799324546e-05, 0,
                1.66362887238597e-04, 0,
                3.82276797905696e-04, 0,
                5.55397776875272e-04, 0,
            },
            8,
            128 * 44100,
            "sdm-8",
            0, 0, 0
        },
        { // 28
            {
                1.18730059129261e+00,
                5.66733317291325e-01,
                1.40117339676942e-01,
                1.87599862200771e-02,
                1.27685506908071e-03,
                8.76397405988154e-06,
                -1.90294986721073e-06,
                -7.39020160622772e-08,
            },
            {
                8.10778762576884e-05, 0,
                6.65340842513387e-04, 0,
                1.52852264942192e-03, 0,
                2.22035724073886e-03, 0,
            },
            8,
            64 * 44100,
            "clans-8",
            0, 0, 0
        },
        { // 29
            {
                7.44453769826547e-01,
                2.69850507860307e-01,
                6.16093616071757e-02,
                9.52771711245796e-03,
                1.02903114196526e-03,
                6.63758229311911e-05,
                2.91124056073927e-06,
                -4.29323230577427e-08,
            },
            {
                8.10778762576884e-05, 0,
                6.65340842513387e-04, 0,
                1.52852264942192e-03, 0,
                2.22035724073886e-03, 0,
            },
            8,
            64 * 44100,
            "sdm-8",
            0, 0, 0
        }, };

    public:
        static double sdm_filter_calc(const double *s, double *d, const sdm_filter_t *f, double x, double y);
        static void sdm_filter_calc2(sdm_state_t *src, sdm_state_t *dst, const sdm_filter_t *f, double x);
};

//-------------------------------------------------------------------------------------------

double SDMTrellisSoxOriginalTester::sdm_filter_calc(const double *s, double *d, const sdm_filter_t *f, double x, double y)
{
    const double *a = f->a;
    const double *g = f->g;
    double v;
    int i;

    d[0] = s[0] - g[0] * s[1] + x - y;
    v = x + a[0] * d[0];

    for (i = 1; i < f->order - 1; i++) {
        d[i] = s[i] + s[i - 1] - g[i] * s[i + 1];
        v += a[i] * d[i];
    }

    d[i] = s[i] + s[i - 1];
    v += a[i] * d[i];

    return v;
}

//-------------------------------------------------------------------------------------------

void SDMTrellisSoxOriginalTester::sdm_filter_calc2(sdm_state_t *src, sdm_state_t *dst, const sdm_filter_t *f, double x)
{
    const double *a = f->a;
    double v;
    int i;

    v = sdm_filter_calc(src->state, dst[0].state, f, x, 0.0);

    for (i = 0; i < f->order; i++)
        dst[1].state[i] = dst[0].state[i];

    dst[0].state[0] += 1.0;
    dst[1].state[0] -= 1.0;

    dst[0].cost = src->cost + sqr(v + a[0]);
    dst[1].cost = src->cost + sqr(v - a[0]);
}

//-------------------------------------------------------------------------------------------

// osr = over sampling ratio
template<class X> X sinusoidalWave(int idx, int wavefreq, int osr)
{
    double pincr = (c_PI_D * wavefreq) / (44100.0 * static_cast<double>(osr));
    double x = sin(pincr * static_cast<double>(idx)) * 0.5;
    return static_cast<X>(x);
}

TEST(SDMTrellisPlain, sinusoidalDSD256_1kHz_SMD_8)
{
    const int c_DSDRate = 256;
    const int c_tone = 1000;

    sdm_state_t tCurr, tNext[2];
    memset(tCurr, 0, sizeof(sdm_state_t));
    memset(tNext[0], 0, sizeof(sdm_state_t));
    memset(tNext[1], 0, sizeof(sdm_state_t));

    for(int i = 0; i < c_DSDRate * c_BaseFrequency; i++)
    {
        double in = sinusoidalWave<double>(i, c_tone, c_DSDRate);
        SDMTrellisSoxOriginalTester::sdm_filter_calc2(&tCurr, tNext, SDMTrellisSoxOriginalTester::sdm_filters[1], in);
    }
}

float dotproduct(float *a, float *d, float x)
{
    for(int i = 0; i < 8; i++)
        x += a[i] * d[i];
}