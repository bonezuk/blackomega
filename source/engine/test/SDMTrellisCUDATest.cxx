#include <cuda_runtime.h>
#include "gtest/gtest.h"

#include "common/inc/Random.h"
#include "common/inc/TimeStamp.h"
#include "engine/inc/SDMTrellisCUDA.h"

using namespace omega;
using namespace omega::engine;

//-------------------------------------------------------------------------------------------

class SDMTrellisCUDATest : public ::testing::Test 
{
protected:
    static void SetUpTestSuite() {
        cuda_init_result_ = initCUDAOmega();

        if (cuda_init_result_ >= 0) {
            std::cout << "[INFO] CUDA device initialized successfully (result = "
                      << cuda_init_result_ << ")." << std::endl;
        } else {
            std::cout << "[WARN] No CUDA device available (initCUDAOmega() returned "
                      << cuda_init_result_ << "). CUDA tests will be skipped." << std::endl;
        }
    }

    void SetUp() override {
        if (cuda_init_result_ < 0) {
            GTEST_SKIP() << "CUDA device not available or initialization failed "
                         << "(initCUDAOmega() returned " << cuda_init_result_ << ")";
        }
    }

    static void TearDownTestSuite() {}

private:
    static int cuda_init_result_;
};

int SDMTrellisCUDATest::cuda_init_result_ = -1;


//-------------------------------------------------------------------------------------------

void testerSineWaveThroughStatesCUDA(SDMTrellisStates<float> *states, SMDTrellisDataCUDA *pCTrellis)
{
    const double c_period = (2.0 * c_PI_D) / (8.0 * c_maxNoSDMTrellisPaths);

    for(int j = 0; j < 8; j++)
    {
        for(int i = 0; i < c_maxNoSDMTrellisPaths; i++)
        {
            int idx = (j * 8) + i + 1;
            float x = static_cast<float>(sin(c_period * static_cast<double>(idx)));
            states->states[j][i] = x;
            pCTrellis->states[0][i][j] = x;
        }
    }
    for(int i = 0; i < c_maxNoSDMTrellisPaths; i++)
    {
        states->cost[i] = 0.0f;
        pCTrellis->cost[i] = 0.0f;
    }
}

//-------------------------------------------------------------------------------------------

TEST_F(SDMTrellisCUDATest, calculateStateAndCostUsingTensorCoreFP32)
{
    constexpr int c_dsdRate = 64;
    constexpr float c_inSample = 0.5f;
    constexpr float c_tolerance = 0.00001f;

    SDMTrellisBlockFilter<float> *filterTest = getSDMTrellisBlockFilter<float>(c_dsdRate, false);
    SDMTrellisStates<float> *statesA = allocateSDMTrellisStates<float>();
    SDMTrellisStates<float> *statesB = allocateSDMTrellisStates<float>();

    SMDTrellisDataCUDA trellisCPU;
    memset(&trellisCPU, 0, sizeof(SMDTrellisDataCUDA));
    SMDTrellisDataCUDA *pTrellisGPU = sdmTrellisAllocateCUDA();
    ASSERT_TRUE(pTrellisGPU != nullptr);

    ASSERT_TRUE(initSDMTrellisFilterCUDA(c_dsdRate, false, &trellisCPU));
    testerSineWaveThroughStatesCUDA(statesA, &trellisCPU);
    trellisCPU.noCandidates = 32;
    ASSERT_TRUE(sdmTrellisCopyDataCUDA(&trellisCPU, pTrellisGPU));

    for(int i = 0; i < 32; i += 8)
    {
        ASSERT_TRUE(sdmCalcTrellisBlockFilter4Lanes(statesA, statesB, filterTest, c_inSample, i));
        ASSERT_TRUE(sdmCalcTrellisBlockFilter4Lanes(statesA, statesB, filterTest, c_inSample, i + 4));
    }

    testSDMTrellisCalcStateAndCostCUDAFP32(pTrellisGPU, c_inSample);
    ASSERT_TRUE(sdmTrellisCopyDataCUDA(pTrellisGPU, &trellisCPU));

    for(int i = 0; i < 32; i++)
    {
        int d = i >> 3;
        int r = i & 0x7;
        int nIdxA = (d << 4) + r;
        int nIdxB = nIdxA + 8;
        EXPECT_NEAR(trellisCPU.cost[(i << 1) + 0], statesB->cost[nIdxA], c_tolerance);
        EXPECT_NEAR(trellisCPU.cost[(i << 1) + 1], statesB->cost[nIdxB], c_tolerance);
        for(int j = 0; j < 8; j++)
        {
            int cIdx = i << 1;
            EXPECT_NEAR(trellisCPU.states[1][cIdx + 0][j], statesB->states[j][nIdxA], c_tolerance);
            EXPECT_NEAR(trellisCPU.states[1][cIdx + 1][j], statesB->states[j][nIdxB], c_tolerance);
        }
    }

    sdmTrellisFreeStatesCUDA(pTrellisGPU);
    freeSDMFreeTrellisBlockFilter<float>(filterTest);
    freeSDMTrellisStates<float>(statesA);
    freeSDMTrellisStates<float>(statesB);
}

//-------------------------------------------------------------------------------------------

TEST_F(SDMTrellisCUDATest, sortRankCostIndicesNoBlanks)
{
    constexpr int noIters = 1000;
    constexpr int max = 64;
    common::Random *rand = common::Random::instance();
    rand->seed(1);

    SMDTrellisDataCUDA trellisCPU;
    memset(&trellisCPU, 0, sizeof(SMDTrellisDataCUDA));
    SMDTrellisDataCUDA *pTrellisGPU = sdmTrellisAllocateCUDA();
    ASSERT_TRUE(pTrellisGPU != nullptr);

    for(int iter = 0; iter < noIters; iter++)
    {
        for(int idx = 0; idx < max; idx++)
        {
            trellisCPU.cost[idx] = static_cast<float>(rand->randomReal2());
        }
        for(int N = 2; N < max; N++)
        {
            trellisCPU.noCandidates = N;
            ASSERT_TRUE(sdmTrellisCopyDataCUDA(&trellisCPU, pTrellisGPU));
            testSDMTrellisRankSortCosts(pTrellisGPU);
            ASSERT_TRUE(sdmTrellisCopyDataCUDA(pTrellisGPU, &trellisCPU));

            for(int idx = 0; idx < N - 1; idx++)
            {
                int iA = trellisCPU.path[idx];
                int iB = trellisCPU.path[idx + 1];
                EXPECT_LE(trellisCPU.cost[iA], trellisCPU.cost[iB]);
            }
        }
    }

    sdmTrellisFreeStatesCUDA(pTrellisGPU);
}

//-------------------------------------------------------------------------------------------
/* Uniform random integer in [0, n), avoiding the bias of plain rand() % n.
   Requires 0 < n <= RAND_MAX. */
//-------------------------------------------------------------------------------------------

int sdmTrellisTestRandBelow(int n)
{
    int limit = RAND_MAX - (RAND_MAX % n);
    int r;
    do {
        r = rand();
    } while (r >= limit);
    return r % n;
}

//-------------------------------------------------------------------------------------------
/* Sets a random number (0 .. n-1) of distinct elements to -1. */
//-------------------------------------------------------------------------------------------

void sdmTrellisTestBlankRandom(uint32_t *a, int n)
{
    if (n <= 0) return;

    int k = sdmTrellisTestRandBelow(n);          /* how many to blank: 0 .. n-1 */
    int remaining = n;

    for (int i = 0; i < n && k > 0; i++, remaining--) {
        if (sdmTrellisTestRandBelow(remaining) < k) {   /* chance k/remaining */
            a[i] = 0xffffffff;
            k--;
        }
    }
}

//-------------------------------------------------------------------------------------------

TEST_F(SDMTrellisCUDATest, sortRankCostIndicesWithBlanks)
{
    constexpr int noIters = 1000;
    constexpr int max = 64;
    common::Random *rand = common::Random::instance();
    rand->seed(1);
    srand(0);

    SMDTrellisDataCUDA trellisCPU;
    memset(&trellisCPU, 0, sizeof(SMDTrellisDataCUDA));
    SMDTrellisDataCUDA *pTrellisGPU = sdmTrellisAllocateCUDA();
    ASSERT_TRUE(pTrellisGPU != nullptr);

    for(int iter = 0; iter < noIters; iter++)
    {
        for(int idx = 0; idx < max; idx++)
        {
            trellisCPU.cost[idx] = static_cast<float>(rand->randomReal2());
        }
        for(int N = 2; N < max; N++)
        {
        	for(int idx = 0; idx < N; idx++)
        	{
        		trellisCPU.path[idx] = idx;
        	}
        	sdmTrellisTestBlankRandom(trellisCPU.path, N);
        	int count = 0;
        	for(int idx = 0; idx < N; idx++)
        	{
        		if(trellisCPU.path[idx] != 0xffffffff)
        		{
        			count++;
        		}
        	}
            trellisCPU.noCandidates = N;
            ASSERT_TRUE(sdmTrellisCopyDataCUDA(&trellisCPU, pTrellisGPU));
            testSDMTrellisRankSortCostsWithBlanks(pTrellisGPU);
            ASSERT_TRUE(sdmTrellisCopyDataCUDA(pTrellisGPU, &trellisCPU));
            
            int i;
            for(i = 0; i < count - 1; i++)
            {
                int iA = trellisCPU.path[i];
                int iB = trellisCPU.path[i + 1];
                EXPECT_LE(trellisCPU.cost[iA], trellisCPU.cost[iB]);
            }
            i++;
            while(i < N)
            {
            	EXPECT_EQ(trellisCPU.path[i], 0xffffffff);
            	i++;
            }
        }
    }

    sdmTrellisFreeStatesCUDA(pTrellisGPU);
}

//-------------------------------------------------------------------------------------------

TEST_F(SDMTrellisCUDATest, minCostIndex)
{
    constexpr int noIters = 1000;
    constexpr int max = 64;
    constexpr float c_tolerance = 0.0000001f;
    common::Random *rand = common::Random::instance();
    rand->seed(1);

    SMDTrellisDataCUDA trellisCPU;
    memset(&trellisCPU, 0, sizeof(SMDTrellisDataCUDA));
    SMDTrellisDataCUDA *pTrellisGPU = sdmTrellisAllocateCUDA();
    ASSERT_TRUE(pTrellisGPU != nullptr);

    for(int iter = 0; iter < noIters; iter++)
    {
        for(int idx = 0; idx < max; idx++)
        {
            trellisCPU.cost[idx] = static_cast<float>(rand->randomReal2());
        }

        for(int N = 2; N < max; N++)
        {
            int minIndex = N;
            float minExpect = 100000.0f;
            for(int idx = 0; idx < N; idx++)
            {
                if(trellisCPU.cost[idx] < minExpect)
                {
                    minExpect = trellisCPU.cost[idx];
                    minIndex = idx;
                }
            }

            trellisCPU.noCandidates = N;
            ASSERT_TRUE(sdmTrellisCopyDataCUDA(&trellisCPU, pTrellisGPU));
            testSDMTrellisMinIndex(pTrellisGPU);
            ASSERT_TRUE(sdmTrellisCopyDataCUDA(pTrellisGPU, &trellisCPU));

            for(int nIdx = 0; nIdx < N; nIdx++)
            {
                EXPECT_EQ(trellisCPU.path[nIdx], minIndex);
            }
        }
    }

    sdmTrellisFreeStatesCUDA(pTrellisGPU);
}

//-------------------------------------------------------------------------------------------

int sdmTrellisQCompareUInt32(const void *a, const void *b)
{
    uint32_t vA = *(const uint32_t *)a;
    uint32_t vB = *(const uint32_t *)b;
    if(vA < vB)
    {
        return -1;
    }
    if(vA > vB)
    {
        return 1;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------

TEST_F(SDMTrellisCUDATest, rankSortPath)
{
    constexpr int noIters = 5000;

    common::Random *rand = common::Random::instance();
    rand->seed(1);

    SMDTrellisDataCUDA trellisCPU;
    memset(&trellisCPU, 0, sizeof(SMDTrellisDataCUDA));
    SMDTrellisDataCUDA *pTrellisGPU = sdmTrellisAllocateCUDA();

    for(int iter = 0; iter < noIters; iter++)
    {
        uint32_t state[32];

        for(int order = 4; order < 30; order++)
        {
            int maxNoOrder = 1 << order;
            uint32_t mask = (1 << order) - 1;

            for(int N = 1; N < 32 && N < maxNoOrder; N++)
            {
                QSet<uint32_t> existing;

                for(int i = 0; i < N;)
                {
                    uint32_t nState = (rand->randomUInt32()) & mask;
                    if(!existing.contains(nState))
                    {
                        state[i++] = nState;
                        existing.insert(nState);
                    }
                }

                uint32_t nextS[64];
                for(int i = 0; i < N; i++)
                {
                    nextS[i << 1] = state[i] << 1;
                    nextS[(i << 1) + 1] = nextS[i << 1] | 0x1;
                }

                int nCand = N << 1;
                for(int i = 0; i < nCand; i++)
                {
                    trellisCPU.path[i] = nextS[i];
                }
                trellisCPU.noCandidates = nCand;
                
                ASSERT_TRUE(sdmTrellisCopyDataCUDA(&trellisCPU, pTrellisGPU));
                testSDMTrellisRankSortCosts(pTrellisGPU);
                ASSERT_TRUE(sdmTrellisCopyDataCUDA(pTrellisGPU, &trellisCPU));

                qsort(nextS, nCand, sizeof(uint32_t), sdmTrellisQCompareUInt32);

                for(int i = 0; i < nCand; i++)
                {
                    EXPECT_EQ(trellisCPU.path[i], nextS[i]);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

TEST_F(SDMTrellisCUDATest, pruneDuplicateStatesWithHigherCost)
{
    constexpr int noIters = 5000;

    common::Random *rand = common::Random::instance();
    rand->seed(1);

    SMDTrellisDataCUDA trellisCPU;
    memset(&trellisCPU, 0, sizeof(SMDTrellisDataCUDA));
    SMDTrellisDataCUDA *pTrellisGPU = sdmTrellisAllocateCUDA();

    for(int iter = 0; iter < noIters; iter++)
    {
    	float cost[64];
    	
    	for(int i = 0; i < 64; i++)
    	{
	    	cost[i] = static_cast<float>(rand->randomReal2());
    	}
    	
    	for(int order = 4; order < 30; order++)
    	{
            int maxNoOrder = 1 << order;
            uint32_t mask = (1 << order) - 1;

            for(int N = 1; N < 32 && N < maxNoOrder; N++)
            {
                QSet<uint32_t> existing;
                
                uint32_t state[32];
                for(int i = 0; i < N;)
                {
                    uint32_t nS = (rand->randomUInt32()) & mask;
                    if(!existing.contains(nS))
                    {
                        state[i++] = nS;
                        existing.insert(nS);
                    }
                }

                uint32_t nextS[64];
                for(int i = 0; i < N; i++)
                {
                    nextS[i << 1] = state[i] << 1;
                    nextS[(i << 1) + 1] = nextS[i << 1] | 0x1;
                }
                int noCandidates = N << 1;
                
                // (state, indices)
                QMap<uint32_t, uint32_t> nMap;
                uint32_t eIndices[64];
                for(int i = 0; i < noCandidates; i++)
                {
                	eIndices[i] = i;
                	auto ppI = nMap.find(nextS[i]);
                	if(ppI != nMap.end())
                	{
                		int iA = ppI.value();
                		int iB = i;
                		int pos = (cost[iA] > cost[iB]) ? iA : iB;
                		eIndices[pos] = 0xffffffff;
                	}
                	nMap.insert(nextS[i], i);
                }
                
                for(int i = 0; i < noCandidates; i++)
                {
	                trellisCPU.cost[i] = cost[i];
	                trellisCPU.path[i] = nextS[i];
                }
                
				trellisCPU.noCandidates = N;
				ASSERT_TRUE(sdmTrellisCopyDataCUDA(&trellisCPU, pTrellisGPU));
				testSDMTrellisPruneStatePairs(pTrellisGPU);
				ASSERT_TRUE(sdmTrellisCopyDataCUDA(pTrellisGPU, &trellisCPU));
				
				for(int i = 0; i < noCandidates; i++)
				{
					EXPECT_EQ(trellisCPU.path[i], eIndices[i]);
				}
            }
    	}    
    }
}

//-------------------------------------------------------------------------------------------

TEST_F(SDMTrellisCUDATest, findNoCandidates)
{
    SMDTrellisDataCUDA trellisCPU;
    memset(&trellisCPU, 0, sizeof(SMDTrellisDataCUDA));
    SMDTrellisDataCUDA *pTrellisGPU = sdmTrellisAllocateCUDA();
    ASSERT_TRUE(pTrellisGPU != nullptr);

	for(int N = 1; N <= 64; N++)
	{
		for(int idx = 0; idx < N; idx++)
		{
			trellisCPU.path[idx] = idx;
		}
		for(int idx = N; idx < 64; idx++)
		{
			trellisCPU.path[idx] = 0xffffffff;
		}
		trellisCPU.noCandidates = N;
		
		ASSERT_TRUE(sdmTrellisCopyDataCUDA(&trellisCPU, pTrellisGPU));
		testSDMTrellisFindNoCandidates(pTrellisGPU);
		ASSERT_TRUE(sdmTrellisCopyDataCUDA(pTrellisGPU, &trellisCPU));
		
		for(int idx = 0; idx < 64; idx++)
		{
			EXPECT_EQ(trellisCPU.path[idx], N);
		}
	}
}

//-------------------------------------------------------------------------------------------

TEST_F(SDMTrellisCUDATest, vibertiStepExample)
{
	struct VTestData {
		int cPath;
		int nPath;
		int nIndex;
		double cost;
	};
	
	const VTestData c_testData[64] = {
		{ 0, 0, 0, 0.4379}, { 16, 0,32, 0.5193},
		{ 0, 1, 1, 0.9190}, { 16, 1,33, 0.7845},
		{ 1, 2, 2, 0.5954}, { 17, 2,34, 0.7977},
		{ 1, 3, 3, 0.4869}, { 17, 3,35, 0.1034},
		{ 2, 4, 4, 0.8977}, { 18, 4,36, 0.6151},
		{ 2, 5, 5, 0.9330}, { 18, 5,37, 0.4602},
		{ 3, 6, 6, 0.3050}, { 19, 6,38, 0.4140},
		{ 3, 7, 7, 0.6166}, { 19, 7,39, 0.2215},
		{ 4, 8, 8, 0.1028}, { 20, 8,40, 0.2124},
		{ 4, 9, 9, 0.2820}, { 20, 9,41, 0.2706},
		{ 5,10,10, 0.1131}, { 21,10,42, 0.7872},
		{ 5,11,11, 0.9250}, { 21,11,43, 0.3019},
		{ 6,12,12, 0.3957}, { 22,12,44, 0.0565},
		{ 6,13,13, 0.2016}, { 22,13,45, 0.1837},
		{ 7,14,14, 0.4069}, { 23,14,46, 0.9169},
		{ 7,15,15, 0.4672}, { 23,15,47, 0.7459},
		{ 8,16,16, 0.1552}, { 24,16,48, 0.5927},
		{ 8,17,17, 0.4564}, { 24,17,49, 0.9989},
		{ 9,18,18, 0.6291}, { 25,18,50, 0.0829},
		{ 9,19,19, 0.7481}, { 25,19,51, 0.4175},
		{10,20,20, 0.8228}, { 26,20,52, 0.2327},
		{10,21,21, 0.8971}, { 26,21,53, 0.7475},
		{11,22,22, 0.0308}, { 27,22,54, 0.1803},
		{11,23,23, 0.5543}, { 27,23,55, 0.5129},
		{12,24,24, 0.8847}, { 28,24,56, 0.2746},
		{12,25,25, 0.6405}, { 28,25,57, 0.1582},
		{13,26,26, 0.9950}, { 29,26,58, 0.5409},
		{13,27,27, 0.3443}, { 29,27,59, 0.4451},
		{14,28,28, 0.9320}, { 30,28,60, 0.7314},
		{14,29,29, 0.1638}, { 30,29,61, 0.1882},
		{15,30,30, 0.2971}, { 31,30,62, 0.6082},
		{15,31,31, 0.6150}, { 31,31,63, 0.9511}
	};

	struct VTestResult {
		int nPath;
		int nIndex;
		double cost;
	};
	const VTestResult c_testResults[32] = {
		{22,19, 0.0308}, {12,38, 0.0565}, 
		{18,49, 0.0829}, { 8, 4, 0.1028}, 
		{ 3,41, 0.1034}, {10, 5, 0.1131}, 
		{16,16, 0.1552}, {25,60, 0.1582}, 
		{29,30, 0.1638}, {13,46, 0.1837}, 
		{ 7,43, 0.2215}, {20,50, 0.2327}, 
		{ 9,44, 0.2706}, {24,52, 0.2746}, 
		{30,23, 0.2971}, {11,45, 0.3019}, 
		{ 6, 3, 0.3050}, {27,29, 0.3443}, 
		{14, 7, 0.4069}, {19,57, 0.4175},
		{ 0, 0, 0.4379}, {17,24, 0.4564},
		{ 5,42, 0.4602}, {15,15, 0.4672},
		{23,59, 0.5129}, {26,53, 0.5409},
		{ 2, 1, 0.5954}, {31,31, 0.6150},
		{ 4,34, 0.6151}, {28,54, 0.7314},
		{21,58, 0.7475}, { 1,40, 0.7845}
	};

    SMDTrellisDataCUDA trellisCPU;
    memset(&trellisCPU, 0, sizeof(SMDTrellisDataCUDA));
    SMDTrellisDataCUDA *pTrellisGPU = sdmTrellisAllocateCUDA();

    for(int i = 0; i < 32; i++)
    {
        trellisCPU.path[i] = i;
    }
    for(int i = 0; i < 64; i++)
    {
        trellisCPU.cost[c_testData[i].nIndex] = static_cast<float>(c_testData[i].cost);
    }
    trellisCPU.order = 5;
    trellisCPU.noCandidates = 32;
    sdmTrellisCopyDataCUDA(&trellisCPU, pTrellisGPU);

    testSDMTrellisVibertiStep(pTrellisGPU);

    sdmTrellisCopyDataCUDA(pTrellisGPU, &trellisCPU);
    for(int i = 0; i < 32; i++)
    {
        EXPECT_EQ(trellisCPU.path[i], c_testResults[i].nPath);
        EXPECT_NEAR(trellisCPU.cost[i], c_testResults[i].cost, 0.00001);
    }
}

//-------------------------------------------------------------------------------------------

TEST_F(SDMTrellisCUDATest, compareProcessRun)
{
    constexpr int dsdRate = 64;
    constexpr int c_numberOfSamples = 1024;
    constexpr int numOutputSamples = c_numberOfSamples / 8;

    float *inSamples = new float [c_numberOfSamples];
    for(int i = 0; i < c_numberOfSamples; i++)
    {
        double ang = (static_cast<double>(i) * 2.0 * c_PI_D) / static_cast<double>(c_numberOfSamples);
        inSamples[i] = sin(ang) * 0.5;
    }

    SMDTrellisDataCUDA trellisCPU;
    memset(&trellisCPU, 0, sizeof(SMDTrellisDataCUDA));
    ASSERT_TRUE(initSDMTrellisFilterCUDA(dsdRate, false, &trellisCPU));
    SMDTrellisDataCUDA *pTrellisGPU = sdmTrellisAllocateCUDA();
    sdmTrellisCopyDataCUDA(&trellisCPU, pTrellisGPU);
    uint8_t *outC = new uint8_t [numOutputSamples];
    ASSERT_TRUE(sdmCUDATrellisRunSingle(pTrellisGPU, inSamples, outC, c_numberOfSamples, true, true));

    SDMTrellis<float> modT;
    ASSERT_TRUE(modT.init(dsdRate, 9, 31));
    uint8_t *outT = new uint8_t [numOutputSamples];
    ASSERT_EQ(modT.process(inSamples, c_numberOfSamples, outT, c_numberOfSamples, true), c_numberOfSamples);

    for(int i = 0; i < numOutputSamples; i++)
    {
        EXPECT_EQ(outC[i], outT[i]);
    }

    delete [] inSamples;
    delete [] outC;
    delete [] outT;
}

//-------------------------------------------------------------------------------------------
