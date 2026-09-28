#include "gtest/gtest.h"

#include "common/inc/CommonTypes.h"
#include "engine/inc/SDMTrellis.h"

#include "hwy/nanobenchmark.h"
#include "hwy/timer.h"

//-------------------------------------------------------------------------------------------

#define SOX_INT_MIN(bits) (1 <<((bits)-1))
#define SOX_INT_MAX(bits) (((unsigned)-1)>>(33-(bits)))
#define SOX_SAMPLE_MAX (sox_sample_t)SOX_INT_MAX(32)
#define SOX_SAMPLE_MIN (sox_sample_t)SOX_INT_MIN(32)

#define MAX_FILTER_ORDER 8
#define PATH_HASH_SIZE 128
#define PATH_HASH_MASK (PATH_HASH_SIZE - 1)

#define SDM_TRELLIS_MAX_ORDER 32
#define SDM_TRELLIS_MAX_NUM   32
#define SDM_TRELLIS_MAX_LAT   2048

#define sqr(a) ((a) * (a))
#define array_length(a) (sizeof(a)/sizeof(a[0]))
#define min(a, b) ((a) <= (b) ? (a) : (b))

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
//-------------------------------------------------------------------------------------------

template<class X> X sinusoidalWave(int idx, int wavefreq, int osr)
{
    double pincr = (c_PI_D * wavefreq) / (44100.0 * static_cast<double>(osr));
    double x = sin(pincr * static_cast<double>(idx)) * 0.25;
    return static_cast<X>(x);
}

//-------------------------------------------------------------------------------------------

using namespace omega::engine;

TEST(SDMTrellis, sinusoidalDSD256_1kHz_Float_4Lanes)
{
    constexpr int c_DSDRate = 256;
    constexpr int c_BaseFrequency = 44100;
    constexpr int c_tone = 1000;
    constexpr double c_Tolerance = 0.1;

    SDMTrellisSoxOriginalTester::sdm_state_t tCurr, tNext[2];
    memset(&tCurr, 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[0], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[1], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));

    SDMTrellisFilter_Float *filter = getSDMTrellisFilter<float>(256, false);
    SDMTrellisState_Float *curr = allocateSMDTrellisStateArray<float>(1);
    SDMTrellisState_Float *next = allocateSMDTrellisStateArray<float>(2);

    bool isSupported = true;
    for(int i = 0; i < c_DSDRate * c_BaseFrequency && isSupported; i++)
    {
        double in = sinusoidalWave<double>(i, c_tone, c_DSDRate);
        SDMTrellisSoxOriginalTester::sdm_filter_calc2(&tCurr, tNext, &SDMTrellisSoxOriginalTester::sdm_filters[1], in);

        isSupported = sdmCalcTrellisFilter4Lanes(curr, next, filter, static_cast<float>(in));
        if(isSupported)
        {
            for(int idx = 0; idx < 2; idx++)
            {
                for(int j = 0; j < 8; j++)
                {
                    ASSERT_TRUE(isEqual(static_cast<double>(next[idx].state[j]), tNext[idx].state[j], c_Tolerance));
                }
                ASSERT_TRUE(isEqual(static_cast<double>(next[idx].cost), tNext[idx].cost, c_Tolerance));
            }

            int lIndex = (tNext[0].cost < tNext[1].cost) ? 0 : 1;
            memcpy(&tCurr, &tNext[lIndex], sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
            for(int i = 0; i < 8; i++)
            {
                curr->state[i] = static_cast<float>(tCurr.state[i]);
            }
            if(tCurr.cost > 1000.0)
            {
                tCurr.cost -= 1000.0;
            }
            curr->cost = static_cast<float>(tCurr.cost);
        }
    }

    freeSDMFreeTrellisStateArray<float>(curr, 1);
    freeSDMFreeTrellisStateArray<float>(next, 2);
    freeSDMFreeTrellisFilter<float>(filter);
}

TEST(SDMTrellis, sinusoidalDSD256_1kHz_Float_4Lanes_Timing)
{
    constexpr int c_DSDRate = 256;
    constexpr int c_BaseFrequency = 44100;
    constexpr int c_tone = 1000;
    constexpr double c_Tolerance = 0.1;

    SDMTrellisSoxOriginalTester::sdm_state_t tCurr, tNext[2];
    memset(&tCurr, 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[0], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[1], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));

    SDMTrellisFilter_Float *filter = getSDMTrellisFilter<float>(256, false);
    SDMTrellisState_Float *curr = allocateSMDTrellisStateArray<float>(1);
    SDMTrellisState_Float *next = allocateSMDTrellisStateArray<float>(2);

    std::vector<double> waveA(c_DSDRate * c_BaseFrequency);
    std::vector<float> waveB(c_DSDRate * c_BaseFrequency);
    for(int i = 0; i < c_DSDRate * c_BaseFrequency; i++)
    {
        waveA[i] = sinusoidalWave<double>(i, c_tone, c_DSDRate);
        waveB[i] = static_cast<float>(waveA[i]);
    }

    double sinkA = 0.0;
    const double tA = hwy::platform::Now();
    for(int i = 0; i < c_DSDRate * c_BaseFrequency; i++)
    {
        SDMTrellisSoxOriginalTester::sdm_filter_calc2(&tCurr, tNext, &SDMTrellisSoxOriginalTester::sdm_filters[1], waveA[i]);
        sinkA += tNext[0].cost + tNext[1].cost;
    }
    const double tB = hwy::platform::Now() - tA;
    hwy::PreventElision(sinkA);

    float sinkB = 0.0f;
    const double tC = hwy::platform::Now();
    for(int i = 0; i < c_DSDRate * c_BaseFrequency; i++)
    {
        sdmCalcTrellisFilter4Lanes(curr, next, filter, waveB[i]);
        sinkB += next[0].cost + next[1].cost;
    }
    const double tD = hwy::platform::Now() - tC;
    hwy::PreventElision(sinkB);

    fprintf(stdout, "original=%.9f, simd=%.9f\n", tB, tD);

    freeSDMFreeTrellisStateArray<float>(curr, 1);
    freeSDMFreeTrellisStateArray<float>(next, 2);
    freeSDMFreeTrellisFilter<float>(filter);
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, sinusoidalDSD256_1kHz_Float_8Lanes)
{
    constexpr int c_DSDRate = 256;
    constexpr int c_BaseFrequency = 44100;
    constexpr int c_tone = 1000;
    constexpr double c_Tolerance = 0.1;

    SDMTrellisSoxOriginalTester::sdm_state_t tCurr, tNext[2];
    memset(&tCurr, 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[0], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[1], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));

    SDMTrellisFilter_Float *filter = getSDMTrellisFilter<float>(256, false);
    SDMTrellisState_Float *curr = allocateSMDTrellisStateArray<float>(1);
    SDMTrellisState_Float *next = allocateSMDTrellisStateArray<float>(2);

    bool isSupported = true;
    for(int i = 0; i < c_DSDRate * c_BaseFrequency && isSupported; i++)
    {
        double in = sinusoidalWave<double>(i, c_tone, c_DSDRate);
        SDMTrellisSoxOriginalTester::sdm_filter_calc2(&tCurr, tNext, &SDMTrellisSoxOriginalTester::sdm_filters[1], in);

        isSupported = sdmCalcTrellisFilter8Lanes(curr, next, filter, static_cast<float>(in));
        if(isSupported)
        {
            for(int idx = 0; idx < 2; idx++)
            {
                for(int j = 0; j < 8; j++)
                {
                    ASSERT_TRUE(isEqual(static_cast<double>(next[idx].state[j]), tNext[idx].state[j], c_Tolerance));
                }
                ASSERT_TRUE(isEqual(static_cast<double>(next[idx].cost), tNext[idx].cost, c_Tolerance));
            }

            int lIndex = (tNext[0].cost < tNext[1].cost) ? 0 : 1;
            memcpy(&tCurr, &tNext[lIndex], sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
            for(int i = 0; i < 8; i++)
            {
                curr->state[i] = static_cast<float>(tCurr.state[i]);
            }
            if(tCurr.cost > 1000.0)
            {
                tCurr.cost -= 1000.0;
            }
            curr->cost = static_cast<float>(tCurr.cost);
        }
    }

    freeSDMFreeTrellisStateArray<float>(curr, 1);
    freeSDMFreeTrellisStateArray<float>(next, 2);
    freeSDMFreeTrellisFilter<float>(filter);
}

TEST(SDMTrellis, sinusoidalDSD256_1kHz_Float_8Lanes_Timing)
{
    constexpr int c_DSDRate = 256;
    constexpr int c_BaseFrequency = 44100;
    constexpr int c_tone = 1000;
    constexpr double c_Tolerance = 0.1;

    SDMTrellisSoxOriginalTester::sdm_state_t tCurr, tNext[2];
    memset(&tCurr, 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[0], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[1], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));

    SDMTrellisFilter_Float *filter = getSDMTrellisFilter<float>(256, false);
    SDMTrellisState_Float *curr = allocateSMDTrellisStateArray<float>(1);
    SDMTrellisState_Float *next = allocateSMDTrellisStateArray<float>(2);

    std::vector<double> waveA(c_DSDRate * c_BaseFrequency);
    std::vector<float> waveB(c_DSDRate * c_BaseFrequency);
    for(int i = 0; i < c_DSDRate * c_BaseFrequency; i++)
    {
        waveA[i] = sinusoidalWave<double>(i, c_tone, c_DSDRate);
        waveB[i] = static_cast<float>(waveA[i]);
    }

    double sinkA = 0.0;
    const double tA = hwy::platform::Now();
    for(int i = 0; i < c_DSDRate * c_BaseFrequency; i++)
    {
        SDMTrellisSoxOriginalTester::sdm_filter_calc2(&tCurr, tNext, &SDMTrellisSoxOriginalTester::sdm_filters[1], waveA[i]);
        sinkA += tNext[0].cost + tNext[1].cost;
    }
    const double tB = hwy::platform::Now() - tA;
    hwy::PreventElision(sinkA);

    float sinkB = 0.0f;
    const double tC = hwy::platform::Now();
    for(int i = 0; i < c_DSDRate * c_BaseFrequency; i++)
    {
        sdmCalcTrellisFilter8Lanes(curr, next, filter, waveB[i]);
        sinkB += next[0].cost + next[1].cost;
    }
    const double tD = hwy::platform::Now() - tC;
    hwy::PreventElision(sinkB);

    fprintf(stdout, "original=%.9f, simd=%.9f\n", tB, tD);

    freeSDMFreeTrellisStateArray<float>(curr, 1);
    freeSDMFreeTrellisStateArray<float>(next, 2);
    freeSDMFreeTrellisFilter<float>(filter);
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, sinusoidalDSD256_1kHz_Double_4Lanes)
{
    constexpr int c_DSDRate = 256;
    constexpr int c_BaseFrequency = 44100;
    constexpr int c_tone = 1000;
    constexpr double c_Tolerance = 0.1;

    SDMTrellisSoxOriginalTester::sdm_state_t tCurr, tNext[2];
    memset(&tCurr, 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[0], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[1], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));

    SDMTrellisFilter_Double *filter = getSDMTrellisFilter<double>(256, false);
    SDMTrellisState_Double *curr = allocateSMDTrellisStateArray<double>(1);
    SDMTrellisState_Double *next = allocateSMDTrellisStateArray<double>(2);

    bool isSupported = true;
    for(int i = 0; i < c_DSDRate * c_BaseFrequency && isSupported; i++)
    {
        double in = sinusoidalWave<double>(i, c_tone, c_DSDRate);
        SDMTrellisSoxOriginalTester::sdm_filter_calc2(&tCurr, tNext, &SDMTrellisSoxOriginalTester::sdm_filters[1], in);

        isSupported = sdmCalcTrellisFilter4Lanes(curr, next, filter, static_cast<float>(in));
        if(isSupported)
        {
            for(int idx = 0; idx < 2; idx++)
            {
                for(int j = 0; j < 8; j++)
                {
                    ASSERT_TRUE(isEqual(next[idx].state[j], tNext[idx].state[j], c_Tolerance));
                }
                ASSERT_TRUE(isEqual(next[idx].cost, tNext[idx].cost, c_Tolerance));
            }

            int lIndex = (tNext[0].cost < tNext[1].cost) ? 0 : 1;
            memcpy(&tCurr, &tNext[lIndex], sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
            for(int i = 0; i < 8; i++)
            {
                curr->state[i] = tCurr.state[i];
            }
            if(tCurr.cost > 1000.0)
            {
                tCurr.cost -= 1000.0;
            }
            curr->cost = tCurr.cost;
        }
    }

    freeSDMFreeTrellisStateArray<double>(curr, 1);
    freeSDMFreeTrellisStateArray<double>(next, 2);
    freeSDMFreeTrellisFilter<double>(filter);
}

TEST(SDMTrellis, sinusoidalDSD256_1kHz_Double_4Lanes_Timing)
{
    constexpr int c_DSDRate = 256;
    constexpr int c_BaseFrequency = 44100;
    constexpr int c_tone = 1000;
    constexpr double c_Tolerance = 0.1;

    SDMTrellisSoxOriginalTester::sdm_state_t tCurr, tNext[2];
    memset(&tCurr, 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[0], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[1], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));

    SDMTrellisFilter_Double *filter = getSDMTrellisFilter<double>(256, false);
    SDMTrellisState_Double *curr = allocateSMDTrellisStateArray<double>(1);
    SDMTrellisState_Double *next = allocateSMDTrellisStateArray<double>(2);

    std::vector<double> waveA(c_DSDRate * c_BaseFrequency);
    std::vector<double> waveB(c_DSDRate * c_BaseFrequency);
    for(int i = 0; i < c_DSDRate * c_BaseFrequency; i++)
    {
        waveA[i] = sinusoidalWave<double>(i, c_tone, c_DSDRate);
        waveB[i] = waveA[i];
    }

    double sinkA = 0.0;
    const double tA = hwy::platform::Now();
    for(int i = 0; i < c_DSDRate * c_BaseFrequency; i++)
    {
        SDMTrellisSoxOriginalTester::sdm_filter_calc2(&tCurr, tNext, &SDMTrellisSoxOriginalTester::sdm_filters[1], waveA[i]);
        sinkA += tNext[0].cost + tNext[1].cost;
    }
    const double tB = hwy::platform::Now() - tA;
    hwy::PreventElision(sinkA);

    double sinkB = 0.0f;
    const double tC = hwy::platform::Now();
    for(int i = 0; i < c_DSDRate * c_BaseFrequency; i++)
    {
        sdmCalcTrellisFilter4Lanes(curr, next, filter, waveB[i]);
        sinkB += next[0].cost + next[1].cost;
    }
    const double tD = hwy::platform::Now() - tC;
    hwy::PreventElision(sinkB);

    fprintf(stdout, "original=%.9f, simd=%.9f\n", tB, tD);

    freeSDMFreeTrellisStateArray<double>(curr, 1);
    freeSDMFreeTrellisStateArray<double>(next, 2);
    freeSDMFreeTrellisFilter<double>(filter);
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, sinusoidalDSD256_1kHz_Double_8Lanes)
{
    constexpr int c_DSDRate = 256;
    constexpr int c_BaseFrequency = 44100;
    constexpr int c_tone = 1000;
    constexpr double c_Tolerance = 0.1;

    SDMTrellisSoxOriginalTester::sdm_state_t tCurr, tNext[2];
    memset(&tCurr, 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[0], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[1], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));

    SDMTrellisFilter_Double *filter = getSDMTrellisFilter<double>(256, false);
    SDMTrellisState_Double *curr = allocateSMDTrellisStateArray<double>(1);
    SDMTrellisState_Double *next = allocateSMDTrellisStateArray<double>(2);

    bool isSupported = true;
    for(int i = 0; i < c_DSDRate * c_BaseFrequency && isSupported; i++)
    {
        double in = sinusoidalWave<double>(i, c_tone, c_DSDRate);
        SDMTrellisSoxOriginalTester::sdm_filter_calc2(&tCurr, tNext, &SDMTrellisSoxOriginalTester::sdm_filters[1], in);

        isSupported = sdmCalcTrellisFilter8Lanes(curr, next, filter, static_cast<float>(in));
        if(isSupported)
        {
            for(int idx = 0; idx < 2; idx++)
            {
                for(int j = 0; j < 8; j++)
                {
                    ASSERT_TRUE(isEqual(next[idx].state[j], tNext[idx].state[j], c_Tolerance));
                }
                ASSERT_TRUE(isEqual(next[idx].cost, tNext[idx].cost, c_Tolerance));
            }

            int lIndex = (tNext[0].cost < tNext[1].cost) ? 0 : 1;
            memcpy(&tCurr, &tNext[lIndex], sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
            for(int i = 0; i < 8; i++)
            {
                curr->state[i] = tCurr.state[i];
            }
            if(tCurr.cost > 1000.0)
            {
                tCurr.cost -= 1000.0;
            }
            curr->cost = tCurr.cost;
        }
    }

    freeSDMFreeTrellisStateArray<double>(curr, 1);
    freeSDMFreeTrellisStateArray<double>(next, 2);
    freeSDMFreeTrellisFilter<double>(filter);
}

TEST(SDMTrellis, sinusoidalDSD256_1kHz_Double_8Lanes_Timing)
{
    constexpr int c_DSDRate = 256;
    constexpr int c_BaseFrequency = 44100;
    constexpr int c_tone = 1000;
    constexpr double c_Tolerance = 0.1;

    SDMTrellisSoxOriginalTester::sdm_state_t tCurr, tNext[2];
    memset(&tCurr, 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[0], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[1], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));

    SDMTrellisFilter_Double *filter = getSDMTrellisFilter<double>(256, false);
    SDMTrellisState_Double *curr = allocateSMDTrellisStateArray<double>(1);
    SDMTrellisState_Double *next = allocateSMDTrellisStateArray<double>(2);

    std::vector<double> waveA(c_DSDRate * c_BaseFrequency);
    std::vector<double> waveB(c_DSDRate * c_BaseFrequency);
    for(int i = 0; i < c_DSDRate * c_BaseFrequency; i++)
    {
        waveA[i] = sinusoidalWave<double>(i, c_tone, c_DSDRate);
        waveB[i] = waveA[i];
    }

    double sinkA = 0.0;
    const double tA = hwy::platform::Now();
    for(int i = 0; i < c_DSDRate * c_BaseFrequency; i++)
    {
        SDMTrellisSoxOriginalTester::sdm_filter_calc2(&tCurr, tNext, &SDMTrellisSoxOriginalTester::sdm_filters[1], waveA[i]);
        sinkA += tNext[0].cost + tNext[1].cost;
    }
    const double tB = hwy::platform::Now() - tA;
    hwy::PreventElision(sinkA);

    double sinkB = 0.0f;
    const double tC = hwy::platform::Now();
    for(int i = 0; i < c_DSDRate * c_BaseFrequency; i++)
    {
        sdmCalcTrellisFilter8Lanes(curr, next, filter, waveB[i]);
        sinkB += next[0].cost + next[1].cost;
    }
    const double tD = hwy::platform::Now() - tC;
    hwy::PreventElision(sinkB);

    fprintf(stdout, "original=%.9f, simd=%.9f\n", tB, tD);

    freeSDMFreeTrellisStateArray<double>(curr, 1);
    freeSDMFreeTrellisStateArray<double>(next, 2);
    freeSDMFreeTrellisFilter<double>(filter);
}

//-------------------------------------------------------------------------------------------

template <typename T> double timeSDMTrellisFilterCalc(bool (*CalcFn)(const SDMTrellisState<T> *, SDMTrellisState<T> *, const SDMTrellisFilter<T> *, T), 
    const std::vector<T>& wave, bool& isSupported)
{
    SDMTrellisFilter<T> *filter = getSDMTrellisFilter<T>(256, false);
    SDMTrellisState<T> *curr = allocateSMDTrellisStateArray<T>(1);
    SDMTrellisState<T> *next = allocateSMDTrellisStateArray<T>(2);

    // Feed the lowest cost path back in as the real trellis does, so each call depends on the last.
    isSupported = true;
    double sinkA = 0.0;
    const double tA = hwy::platform::Now();
    for(int j = 0; j < 10; j++)
    {
		for(size_t i = 0; i < wave.size() && isSupported; i++)
		{
			isSupported = CalcFn(curr, next, filter, wave[i]);
			sinkA += next[0].cost + next[1].cost;
		}
    }
    const double tB = hwy::platform::Now() - tA;
    hwy::PreventElision(sinkA);

    freeSDMFreeTrellisStateArray<T>(curr, 1);
    freeSDMFreeTrellisStateArray<T>(next, 2);
    freeSDMFreeTrellisFilter<T>(filter);
    return tB;
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, sinusoidalDSD256_1kHz_TimingPerTarget)
{
    constexpr int c_DSDRate = 256;
    constexpr int c_BaseFrequency = 44100;
    constexpr int c_tone = 1000;

    std::vector<double> waveD(c_DSDRate * c_BaseFrequency);
    std::vector<float> waveF(c_DSDRate * c_BaseFrequency);
    for(int i = 0; i < c_DSDRate * c_BaseFrequency; i++)
    {
        waveD[i] = sinusoidalWave<double>(i, c_tone, c_DSDRate);
        waveF[i] = static_cast<float>(waveD[i]);
    }

    SDMTrellisSoxOriginalTester::sdm_state_t tCurr, tNext[2];
    memset(&tCurr, 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[0], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));
    memset(&tNext[1], 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t));

    double sinkA = 0.0;
    const double tA = hwy::platform::Now();
    for(int j = 0; j < 10; j++)
    {
        for(size_t i = 0; i < waveD.size(); i++)
        {
            SDMTrellisSoxOriginalTester::sdm_filter_calc2(&tCurr, tNext, &SDMTrellisSoxOriginalTester::sdm_filters[1], waveD[i]);
            sinkA += tNext[0].cost + tNext[1].cost;
        }
    }
    const double tOriginal = hwy::platform::Now() - tA;
    hwy::PreventElision(sinkA);
    fprintf(stdout, "%-8s %-14s %.6fs\n", "", "original", tOriginal);

    auto report = [tOriginal](const char *targetName, const char *fnName, double t, bool isSupported) {
        if(isSupported)
        {
            fprintf(stdout, "%-8s %-14s %.6fs  x%.2f\n", targetName, fnName, t, tOriginal / t);
        }
        else
        {
            fprintf(stdout, "%-8s %-14s not supported\n", targetName, fnName);
        }
    };

    for(int64_t target : sdmTrellisSupportedTargets())
    {
        sdmTrellisSetTarget(target);
        const char *name = sdmTrellisTargetName(target);
        bool isSupported;
        double t;

        t = timeSDMTrellisFilterCalc<float>(sdmCalcTrellisFilter4Lanes, waveF, isSupported);
        report(name, "float x4", t, isSupported);
        t = timeSDMTrellisFilterCalc<float>(sdmCalcTrellisFilter8Lanes, waveF, isSupported);
        report(name, "float x8", t, isSupported);
        t = timeSDMTrellisFilterCalc<double>(sdmCalcTrellisFilter4Lanes, waveD, isSupported);
        report(name, "double x4", t, isSupported);
        t = timeSDMTrellisFilterCalc<double>(sdmCalcTrellisFilter8Lanes, waveD, isSupported);
        report(name, "double x8", t, isSupported);
    }
    sdmTrellisSetTarget(0);
}

//-------------------------------------------------------------------------------------------
