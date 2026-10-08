#include "gtest/gtest.h"
#include <QSet>

#include "common/inc/CommonTypes.h"
#include "common/inc/Random.h"
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

using namespace omega;
using namespace omega::engine;

//-------------------------------------------------------------------------------------------

template <typename T> void testerSineWaveThroughStates(SDMTrellisStates<T> *states, SDMTrellisSoxOriginalTester::sdm_state_t *soxStates)
{
    const double c_period = (2.0 * c_PI_D) / (8.0 * c_maxNoSDMTrellisPaths);

    for(int j = 0; j < 8; j++)
    {
        for(int i = 0; i < c_maxNoSDMTrellisPaths; i++)
        {
            int idx = (j * 8) + i + 1;
            T x = static_cast<T>(sin(c_period * static_cast<double>(idx)));
            states->states[j][i] = x;
            if(i < 8)
            {
                soxStates[i].state[j] = x;
            }
        }
    }
    for(int i = 0; i < c_maxNoSDMTrellisPaths; i++)
    {
        states->cost[i] = static_cast<T>(0.0);
    }
}

//-------------------------------------------------------------------------------------------

template <typename T> void sinusoidalDSD256BlockFilter_4Lanes()
{
    constexpr int c_DSDRate = 256;
    constexpr T c_Tolerance = (T)(0.00001);

    SDMTrellisSoxOriginalTester::sdm_state_t tStateA[16], tStateB[16];
    memset(tStateA, 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t) * 16);
    memset(tStateB, 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t) * 16);

    SDMTrellisBlockFilter<T> *filter = getSDMTrellisBlockFilter<T>(256, false);
    SDMTrellisStates<T> *statesA = allocateSDMTrellisStates<T>();
    SDMTrellisStates<T> *statesB = allocateSDMTrellisStates<T>();

    testerSineWaveThroughStates<T>(statesA, tStateA);

    float inSamples[2] = { 0.5f, 0.25f };
    constexpr int pathBlockMap[16] = { 0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15 };

    for(int i = 0 ; i < 2; i++)
    {
        SDMTrellisStates<T> *curr = statesA;
        SDMTrellisStates<T> *next = statesB;
        SDMTrellisSoxOriginalTester::sdm_state_t *tCurr = (i == 0) ? tStateA : tStateB;
        SDMTrellisSoxOriginalTester::sdm_state_t *tNext = (i == 0) ? tStateB : tStateA;
        for(int j = 0; j < 8; j++)
        {
            SDMTrellisSoxOriginalTester::sdm_filter_calc2(&tCurr[j], &tNext[j << 1], &SDMTrellisSoxOriginalTester::sdm_filters[1], inSamples[i]);
        }
        
        bool isSupported = sdmCalcTrellisBlockFilter4Lanes(curr, next, filter, inSamples[i], 0);
        if(!isSupported)
            GTEST_SKIP() << "No SIMD instruction set for 4 lanes";
        sdmCalcTrellisBlockFilter4Lanes(curr, next, filter, inSamples[i], 4);

        int pathT, pathA;
        for(pathT = 0; pathT < 16; pathT++)
        {
            pathA = pathBlockMap[pathT];
            for(int j = 0; j < 8; j++)
            {
                EXPECT_NEAR(next->states[j][pathA], tNext[pathT].state[j], c_Tolerance);
            }
            EXPECT_NEAR(next->cost[pathA], tNext[pathT].cost, c_Tolerance);
        }
        for(pathT = 0; pathT < 8; pathT++)
        {
            pathA = pathBlockMap[pathT];
            for(int j = 0; j < 8; j++)
            {
                curr->states[j][pathT] = next->states[j][pathA];
            }
            curr->cost[pathT] = next->cost[pathA];
        }
    }

    freeSDMFreeTrellisBlockFilter<T>(filter);
    freeSDMTrellisStates<T>(statesA);
    freeSDMTrellisStates<T>(statesB);
}

//-------------------------------------------------------------------------------------------

template <typename T> void sinusoidalDSD256BlockFilter_8Lanes()
{
    constexpr int c_DSDRate = 256;
    constexpr float c_Tolerance = 0.00001f;

    SDMTrellisSoxOriginalTester::sdm_state_t tStateA[16], tStateB[16];
    memset(tStateA, 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t) * 16);
    memset(tStateB, 0, sizeof(SDMTrellisSoxOriginalTester::sdm_state_t) * 16);

    SDMTrellisBlockFilter<T> *filter = getSDMTrellisBlockFilter<T>(256, false);
    SDMTrellisStates<T> *statesA = allocateSDMTrellisStates<T>();
    SDMTrellisStates<T> *statesB = allocateSDMTrellisStates<T>();

    testerSineWaveThroughStates(statesA, tStateA);

    float inSamples[2] = { 0.5f, 0.25f };
    constexpr int pathBlockMap[16] = { 0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15 };

    for(int i = 0 ; i < 2; i++)
    {
        SDMTrellisStates<T> *curr = statesA;
        SDMTrellisStates<T> *next = statesB;
        SDMTrellisSoxOriginalTester::sdm_state_t *tCurr = (i == 0) ? tStateA : tStateB;
        SDMTrellisSoxOriginalTester::sdm_state_t *tNext = (i == 0) ? tStateB : tStateA;
        for(int j = 0; j < 8; j++)
        {
            SDMTrellisSoxOriginalTester::sdm_filter_calc2(&tCurr[j], &tNext[j << 1], &SDMTrellisSoxOriginalTester::sdm_filters[1], inSamples[i]);
        }
        
        bool isSupported = sdmCalcTrellisBlockFilter8Lanes(curr, next, filter, inSamples[i], 0);
        if(!isSupported)
            GTEST_SKIP() << "No SIMD instruction set for 8 lanes";

        int pathT, pathA;
        for(pathT = 0; pathT < 16; pathT++)
        {
            pathA = pathBlockMap[pathT];
            for(int j = 0; j < 8; j++)
            {
                EXPECT_NEAR(next->states[j][pathA], tNext[pathT].state[j], c_Tolerance);
            }
            EXPECT_NEAR(next->cost[pathA], tNext[pathT].cost, c_Tolerance);
        }
        for(pathT = 0; pathT < 8; pathT++)
        {
            pathA = pathBlockMap[pathT];
            for(int j = 0; j < 8; j++)
            {
                curr->states[j][pathT] = next->states[j][pathA];
            }
            curr->cost[pathT] = next->cost[pathA];
        }
    }

    freeSDMFreeTrellisBlockFilter<T>(filter);
    freeSDMTrellisStates<T>(statesA);
    freeSDMTrellisStates<T>(statesB);
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, sinusoidalDSD256BlockFilter_Float_4Lanes)
{
	sinusoidalDSD256BlockFilter_4Lanes<float>();
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, sinusoidalDSD256BlockFilter_Float_8Lanes)
{
	sinusoidalDSD256BlockFilter_8Lanes<float>();
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, sinusoidalDSD256BlockFilter_Double_4Lanes)
{
	sinusoidalDSD256BlockFilter_4Lanes<double>();
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, sinusoidalDSD256BlockFilter_Double_8Lanes)
{
	sinusoidalDSD256BlockFilter_8Lanes<double>();
}

//-------------------------------------------------------------------------------------------

template <typename T> double timeSDMTrellisFilterBlockCalc(
    bool (*CalcFn)(const SDMTrellisStates<T> *, SDMTrellisStates<T> *, const SDMTrellisBlockFilter<T> *, T, int), 
    const std::vector<T>& wave, bool& isSupported, bool is4Lanes)
{
    SDMTrellisBlockFilter<T> *filter = getSDMTrellisBlockFilter<T>(256, false);
    SDMTrellisStates<T> *curr = allocateSDMTrellisStates<T>();
    SDMTrellisStates<T> *next = allocateSDMTrellisStates<T>();

    isSupported = true;
    double sinkA = 0.0;
    const double tA = hwy::platform::Now();
    for(int j = 0; j < 10; j++)
    {
		for(size_t i = 0; i < wave.size() && isSupported; i++)
		{
			isSupported = CalcFn(curr, next, filter, wave[i], 0);
            if(is4Lanes)
                CalcFn(curr, next, filter, wave[i], 4);
		}
    }
    const double tB = hwy::platform::Now() - tA;

    freeSDMFreeTrellisBlockFilter<T>(filter);
    freeSDMTrellisStates<T>(curr);
    freeSDMTrellisStates<T>(next);

    return tB;
}

//-------------------------------------------------------------------------------------------


TEST(SDMTrellis, sinusoidalDSD256BlockFilter_TimingPerTarget)
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
            for(int k = 0 ; k < 8; k++)
            {
				SDMTrellisSoxOriginalTester::sdm_filter_calc2(&tCurr, tNext, &SDMTrellisSoxOriginalTester::sdm_filters[1], waveD[i]);
				sinkA += tNext[0].cost + tNext[1].cost;
            }
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

        t = timeSDMTrellisFilterBlockCalc<float>(sdmCalcTrellisBlockFilter4Lanes, waveF, isSupported, true);
        report(name, "float x4", t, isSupported);
        t = timeSDMTrellisFilterBlockCalc<float>(sdmCalcTrellisBlockFilter8Lanes, waveF, isSupported, false);
        report(name, "float x8", t, isSupported);
        t = timeSDMTrellisFilterBlockCalc<double>(sdmCalcTrellisBlockFilter4Lanes, waveD, isSupported, true);
        report(name, "double x4", t, isSupported);
        t = timeSDMTrellisFilterBlockCalc<double>(sdmCalcTrellisBlockFilter8Lanes, waveD, isSupported, false);
        report(name, "double x8", t, isSupported);
    }
    sdmTrellisSetTarget(0);
}

//-------------------------------------------------------------------------------------------

template <typename T> class SDMTrellisTester : public SDMTrellis<T>
{
	public:
		SDMTrellisTester();
		virtual ~SDMTrellisTester();
		virtual bool testIsRateSupported(int rate) const;
		virtual int testCurrentIndexFromNext(int nextPathIdx) const;
		
		SDMTrellisStates<T> *testGetStates(int stateIdx);
		uint8_t *pathHashIndex();
		virtual uint32_t testCurrentPath(int pathIdx) const;
		virtual uint32_t testCurrentTrellisState(int pathIdx) const;
		virtual uint32_t testNextPath(int pathIdx) const;
		virtual uint32_t testNextTrellisState(int pathIdx) const;
		virtual int testOutputFromCurrent(int pathIdx) const;
		virtual int testOutputFromNext(int pathIdx) const;
		virtual void testStepPath();
		virtual void testCalc(T sample);
		virtual T testStepMinCostAndResetHash(int& minIdx);
};

//-------------------------------------------------------------------------------------------

template <typename T> SDMTrellisTester<T>::SDMTrellisTester()
{}

//-------------------------------------------------------------------------------------------

template <typename T> SDMTrellisTester<T>::~SDMTrellisTester()
{}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellisTester<T>::testCurrentIndexFromNext(int nextPathIdx) const
{
    return this->currentIndexFromNext(nextPathIdx);
}

//-------------------------------------------------------------------------------------------

template <typename T> bool SDMTrellisTester<T>::testIsRateSupported(int rate) const
{
	return this->isRateSupported(rate);
}

//-------------------------------------------------------------------------------------------

template <typename T> SDMTrellisStates<T> *SDMTrellisTester<T>::testGetStates(int stateIdx)
{
	return this->m_states[stateIdx];
}

//-------------------------------------------------------------------------------------------

template <typename T> uint8_t *SDMTrellisTester<T>::pathHashIndex()
{
	return this->m_pathHashTable;
}

//-------------------------------------------------------------------------------------------

template <typename T> uint32_t SDMTrellisTester<T>::testCurrentPath(int pathIdx) const
{
	return this->currentPath(pathIdx);
}

//-------------------------------------------------------------------------------------------

template <typename T> uint32_t SDMTrellisTester<T>::testCurrentTrellisState(int pathIdx) const
{
	return this->currentTrellisState(pathIdx);
}

//-------------------------------------------------------------------------------------------

template <typename T> uint32_t SDMTrellisTester<T>::testNextPath(int pathIdx) const
{
	return this->nextPath(pathIdx);
}

//-------------------------------------------------------------------------------------------

template <typename T> uint32_t SDMTrellisTester<T>::testNextTrellisState(int pathIdx) const
{
	return this->nextTrellisState(pathIdx);
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellisTester<T>::testOutputFromCurrent(int pathIdx) const
{
	return this->outputFromCurrent(pathIdx);
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellisTester<T>::testOutputFromNext(int pathIdx) const
{
	return this->outputFromNext(pathIdx);
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellisTester<T>::testStepPath()
{
	this->stepPath();
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellisTester<T>::testCalc(T sample)
{
	this->calc(sample);
}

//-------------------------------------------------------------------------------------------

template <typename T> T SDMTrellisTester<T>::testStepMinCostAndResetHash(int& minIdx)
{
	return this->stepMinCostAndResetHash(minIdx);
}

//-------------------------------------------------------------------------------------------

template <typename T> void testSDMTrellisIsRateSupported()
{
	SDMTrellisTester<T> sdmTrellis;
	EXPECT_TRUE(sdmTrellis.testIsRateSupported(64));
	EXPECT_TRUE(sdmTrellis.testIsRateSupported(128));
	EXPECT_TRUE(sdmTrellis.testIsRateSupported(256));
	// TODO: implement the 512 and 1024 rates
	EXPECT_FALSE(sdmTrellis.testIsRateSupported(512));
	EXPECT_FALSE(sdmTrellis.testIsRateSupported(1024));
	
	EXPECT_FALSE(sdmTrellis.testIsRateSupported(32));
	EXPECT_FALSE(sdmTrellis.testIsRateSupported(63));
	EXPECT_FALSE(sdmTrellis.testIsRateSupported(65));
	EXPECT_FALSE(sdmTrellis.testIsRateSupported(127));
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, isRateSupportedFloat)
{
	testSDMTrellisIsRateSupported<float>();
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, isRateSupportedDouble)
{
	testSDMTrellisIsRateSupported<double>();
}

//-------------------------------------------------------------------------------------------
/*
latency = 7
order = 4

idx = 0: (010)0000 0-> (100)0000 :nIdx=0  (0x20, 0x40) (0x0, 0x0) =0
                   1-> (100)0001 :nIdx=8  (    , 0x41) (0x0, 0x1) =0
idx = 1: (011)0001 0-> (110)0010 :nIdx=1  (0x31, 0x62) (0x1, 0x2) =0
                   1-> (110)0011 :nIdx=9  (    , 0x63) (0x1, 0x3) =0
idx = 2: (010)1000 0-> (101)0000 :nIdx=2  (0x28, 0x50) (0x8, 0x0) =0
                   1-> (101)0001 :nIdx=10 (    , 0x51) (0x8, 0x1) =0
idx = 3: (011)1001 0-> (111)0010 :nIdx=3  (0x39, 0x72) (0x9, 0x2) =0
                   1-> (111)0011 :nIdx=11 (    , 0x73) (0x9, 0x3) =0
idx = 4: (111)0010 0-> (110)0100 :nIdx=4  (0x72, 0x64) (0x2, 0x4) =1
                   1-> (110)0101 :nIdx=12 (    , 0x65) (0x2, 0x5) =1
idx = 5: (000)1111 0-> (001)1110 :nIdx=5  (0x0F, 0x1E) (0xF, 0xE) =0
                   1-> (001)1111 :nIdx=13 (    , 0x1F) (0xF, 0xF) =0
idx = 6: (010)1010 0-> (101)0100 :nIdx=6  (0x2A, 0x54) (0xA, 0x4) =0
                   1-> (101)0101 :nIdx=14 (    , 0x55) (0xA, 0x5) =0
idx = 7: (101)0101 0-> (010)1010 :nIdx=7  (0x55, 0x2A) (0x5, 0xA) =1
                   1-> (010)1011 :nIdx=15 (    , 0x2B) (0x5, 0xB) =1
*/
//-------------------------------------------------------------------------------------------

template <typename T> void testSDMTrellisPathStepWithOrder4Latency7()
{
	constexpr uint32_t c_testPaths[8] = { 0x20, 0x31, 0x28, 0x39, 0x72, 0x0F, 0x2A, 0x55 };
	constexpr uint32_t c_testStates[8]= { 0x00, 0x01, 0x08, 0x09, 0x02, 0x0F, 0x0A, 0x05 };
	constexpr int c_testOut[8] = { 0, 0, 0, 0, 1, 0, 0, 1 };
	constexpr uint32_t c_expectNextPaths[16] = {
		0x40, 0x62, 0x50, 0x72, 0x64, 0x1E, 0x54, 0x2A,
        0x41, 0x63, 0x51, 0x73, 0x65, 0x1F, 0x55, 0x2B
	};
	constexpr uint32_t c_expectStates[16] = {
		0x00, 0x02, 0x00, 0x02, 0x04, 0x0E, 0x04, 0x0A,
        0x01, 0x03, 0x01, 0x03, 0x05, 0x0F, 0x05, 0x0B,
	};
    constexpr int c_expectOut[16] = {
        0, 0, 0, 0, 1, 0, 0, 1,
        0, 0, 0, 0, 1, 0, 0, 1
	};
	
	int idx;
    SDMTrellisTester<T> sdm;
    ASSERT_TRUE(sdm.init(64, 4, 7));
    SDMTrellisStates<T> *curr = sdm.testGetStates(0);
	for(idx = 0; idx < 8; idx++)
	{
		curr->path[idx] = c_testPaths[idx];
	}
	
    sdm.testStepPath();
	
	for(idx = 0; idx < 8; idx++)
	{
		EXPECT_EQ(sdm.testCurrentPath(idx), c_testPaths[idx]);
		EXPECT_EQ(sdm.testCurrentTrellisState(idx), c_testStates[idx]);
		EXPECT_EQ(sdm.testOutputFromCurrent(idx), c_testOut[idx]);
	}
	for(idx = 0; idx < 16; idx++)
	{
		EXPECT_EQ(sdm.testNextPath(idx), c_expectNextPaths[idx]);
		EXPECT_EQ(sdm.testNextTrellisState(idx), c_expectStates[idx]);
		EXPECT_EQ(sdm.testOutputFromNext(idx), c_expectOut[idx]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, PathStepWithOrder4Latency7_Float)
{
	testSDMTrellisPathStepWithOrder4Latency7<float>();
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, PathStepWithOrder4Latency7_Double)
{
	testSDMTrellisPathStepWithOrder4Latency7<double>();
}

//-------------------------------------------------------------------------------------------

template <typename T> void testSDMTrellisCurrentIndexFromNext()
{
	SDMTrellisTester<T> sdmTrellis;
	// 0, 8  -> 0 
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(0), 0);
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(8), 0);
	// 1, 9  -> 1
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(1), 1);
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(9), 1);
	// 2,10  -> 2
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(2), 2);
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(10), 2);
	// 3,11  -> 3
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(3), 3);
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(11), 3);
	// 4,12  -> 4
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(4), 4);
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(12), 4);
	// 5,13  -> 5
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(5), 5);
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(13), 5);
	// 6,14  -> 6
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(6), 6);
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(14), 6);
	// 7,15  -> 7
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(7), 7);
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(15), 7);
	
	// 16, 24 -> 8
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(16), 8);
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(24), 8);
	// 23, 31 -> 15
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(23), 15);
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(31), 15);
	
	// 32, 40 -> 16
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(32), 16);
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(40), 16);
	// 37, 45 -> 21
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(37), 21);
	EXPECT_EQ(sdmTrellis.testCurrentIndexFromNext(45), 21);
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, currentIndexFromNext_Float)
{
	testSDMTrellisCurrentIndexFromNext<float>();
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, currentIndexFromNext_Double)
{
	testSDMTrellisCurrentIndexFromNext<double>();
}

//-------------------------------------------------------------------------------------------

template <typename T> uint32_t *testerSDMTrellisGeneratePathsWith12Order(SDMTrellisTester<T>& sdmTrellis, QSet<uint32_t>& pathSet)
{
    uint32_t *hash = new uint32_t [c_maxNoSDMTrellisPaths];
    EXPECT_TRUE(hash != nullptr);
	common::Random *rand = common::Random::instance();
    EXPECT_TRUE(rand != nullptr);
	SDMTrellisStates<T> *curr = sdmTrellis.testGetStates(0);
    EXPECT_TRUE(curr != nullptr);
	for(int idx = 0; idx < c_maxNoSDMTrellisPaths;)
	{
		uint32_t p = rand->randomUInt32() & 0x00ffffff;
		uint32_t s = p & 0x00000fff;
		auto ppI = pathSet.find(s);
		if(ppI == pathSet.end())
		{
			curr->path[idx] = p;
			pathSet.insert(s);
			hash[idx] = p;
			idx++;
		}
	}
	return hash;
}

//-------------------------------------------------------------------------------------------

template <typename T> uint32_t *testerSDMTrellisGeneratePathsWith12Order(SDMTrellisTester<T>& sdmTrellis)
{
    QSet<uint32_t> pathSet;
	return testerSDMTrellisGeneratePathsWith12Order(sdmTrellis, pathSet);
}

//-------------------------------------------------------------------------------------------

template <typename T> void testSDMTrellisStepPath()
{
	SDMTrellisTester<T> sdmTrellis;
	ASSERT_TRUE(sdmTrellis.init(256, 12, 24));
	ASSERT_EQ(sdmTrellis.rate(), 256);
	ASSERT_EQ(sdmTrellis.order(), 12);
	ASSERT_EQ(sdmTrellis.latency(), 24);

	uint32_t *testPaths = testerSDMTrellisGeneratePathsWith12Order<T>(sdmTrellis);
	sdmTrellis.testStepPath();
	
	SDMTrellisStates<T> *next = sdmTrellis.testGetStates(1);
	ASSERT_TRUE(next != nullptr);
	
	for(int cIdx = 0; cIdx < c_maxNoSDMTrellisPaths; cIdx++)
	{
		int d = cIdx >> 3;
		int r = cIdx & 0x7;
		int nIdxA = (d << 4) + r;
		int nIdxB = nIdxA + 8;
		uint32_t nPathA = (testPaths[cIdx] << 1) & 0x00ffffff;
		uint32_t nPathB = nPathA + 1;
		EXPECT_EQ(next->path[nIdxA], nPathA);
		EXPECT_EQ(next->path[nIdxB], nPathB);
	}
	
	delete [] testPaths;
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, stepPathFloat)
{
	testSDMTrellisStepPath<float>();
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, stepPathDouble)
{
	testSDMTrellisStepPath<double>();
}

//-------------------------------------------------------------------------------------------

template <typename T> void testerSineWaveThroughStatesB(SDMTrellisStates<T> *states)
{
    const double c_period = (2.0 * c_PI_D) / (8.0 * c_maxNoSDMTrellisPaths);

    for(int j = 0; j < 8; j++)
    {
        for(int i = 0; i < c_maxNoSDMTrellisPaths; i++)
        {
            int idx = (j * 8) + i + 1;
            T x = static_cast<T>(sin(c_period * static_cast<double>(idx)));
            states->states[j][i] = x;
        }
    }
}

//-------------------------------------------------------------------------------------------

template <typename T> void testSDMTrellisMinPathAndHashReset()
{
	SDMTrellisTester<T> sdmTrellis;
	ASSERT_TRUE(sdmTrellis.init(256, 12, 24));
	ASSERT_EQ(sdmTrellis.rate(), 256);
	ASSERT_EQ(sdmTrellis.order(), 12);
	ASSERT_EQ(sdmTrellis.latency(), 24);

    SDMTrellisStates<T> *curr = sdmTrellis.testGetStates(0);
    SDMTrellisStates<T> *next = sdmTrellis.testGetStates(1);
    testerSineWaveThroughStatesB(curr);
	QSet<uint32_t> pathSet;
	uint32_t *expectPath = testerSDMTrellisGeneratePathsWith12Order(sdmTrellis);
	delete [] expectPath;
	
	int minIdxExpect = 0;
	T minExpect = next->cost[0];
	for(int idx = 0; idx < 2 * c_maxNoSDMTrellisPaths; idx++)
	{
        if(next->cost[idx] < minExpect)
		{
            minExpect = next->cost[idx];
			minIdxExpect = idx;
		}
	}
	
	uint8_t *hashIndex = sdmTrellis.pathHashIndex();
	for(int idx = 0; idx < c_maxNoSDMTrellisPaths; idx++)
	{
		hashIndex[idx] = -2;
	}
	
	int minIdx = -1;
    T min = sdmTrellis.testStepMinCostAndResetHash(minIdx);
	ASSERT_NEAR(min, minExpect, 0.00000001);
	ASSERT_EQ(minIdx, minIdxExpect);
	
	for(const auto& v : pathSet)
	{
		EXPECT_EQ(hashIndex[v], 0);
	}
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, minPathAndHashResetFloat)
{
	testSDMTrellisMinPathAndHashReset<float>();
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, minPathAndHashResetDouble)
{
	testSDMTrellisMinPathAndHashReset<float>();
}

//-------------------------------------------------------------------------------------------
