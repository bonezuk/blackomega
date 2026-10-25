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

TEST_F(SDMTrellisCUDATest, calculateStateAndCostUsingTensorCoreTF32)
{
    constexpr int c_dsdRate = 64;
    constexpr float c_inSample = 0.5f;
    constexpr float c_tolerance = 0.01f;

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

    testSDMTrellisCalcStateAndCostCUDATF32(pTrellisGPU, c_inSample);
    ASSERT_TRUE(sdmTrellisCopyDataCUDA(pTrellisGPU, &trellisCPU));

    for(int i = 0; i < 32; i++)
    {
        int d = i >> 3;
        int r = i & 0x7;
        int nIdxA = (d << 4) + r;
        int nIdxB = nIdxA + 8;
        EXPECT_NEAR(trellisCPU.cost[i], statesB->cost[nIdxA], c_tolerance);
        EXPECT_NEAR(trellisCPU.cost[i+32], statesB->cost[nIdxB], c_tolerance);
        for(int j = 0; j < 8; j++)
        {
            EXPECT_NEAR(trellisCPU.states[1][i][j], statesB->states[j][nIdxA], c_tolerance);
            EXPECT_NEAR(trellisCPU.states[1][i+32][j], statesB->states[j][nIdxB], c_tolerance);
        }
    }

    sdmTrellisFreeStatesCUDA(pTrellisGPU);
    freeSDMFreeTrellisBlockFilter<float>(filterTest);
    freeSDMTrellisStates<float>(statesA);
    freeSDMTrellisStates<float>(statesB);
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
        EXPECT_NEAR(trellisCPU.cost[i], statesB->cost[nIdxA], c_tolerance);
        EXPECT_NEAR(trellisCPU.cost[i+32], statesB->cost[nIdxB], c_tolerance);
        for(int j = 0; j < 8; j++)
        {
            EXPECT_NEAR(trellisCPU.states[1][i][j], statesB->states[j][nIdxA], c_tolerance);
            EXPECT_NEAR(trellisCPU.states[1][i+32][j], statesB->states[j][nIdxB], c_tolerance);
        }
    }

    sdmTrellisFreeStatesCUDA(pTrellisGPU);
    freeSDMFreeTrellisBlockFilter<float>(filterTest);
    freeSDMTrellisStates<float>(statesA);
    freeSDMTrellisStates<float>(statesB);
}

//-------------------------------------------------------------------------------------------

void sdmTrellisTestSortCostIndices(bool isMerge)
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
            if(isMerge)
            {
                testSDMTrellisMergeSortCosts(pTrellisGPU);
            }
            else
            {
                testSDMTrellisRankSortCosts(pTrellisGPU);
            }
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

TEST_F(SDMTrellisCUDATest, sortMergeCostIndices)
{
    sdmTrellisTestSortCostIndices(true);
}

//-------------------------------------------------------------------------------------------

TEST_F(SDMTrellisCUDATest, sortRankCostIndices)
{
    sdmTrellisTestSortCostIndices(false);
}

//-------------------------------------------------------------------------------------------

void sdmTrellisTestMinCost(bool isOld)
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
            float minExpect = 100000.0f;
            for(int idx = 0; idx < N; idx++)
            {
                if(trellisCPU.cost[idx] < minExpect)
                {
                    minExpect = trellisCPU.cost[idx];
                }
            }

            trellisCPU.noCandidates = N;
            ASSERT_TRUE(sdmTrellisCopyDataCUDA(&trellisCPU, pTrellisGPU));
            if(isOld)
            {
                testSDMTrellisMinCostsOld(pTrellisGPU);
            }
            else
            {
                testSDMTrellisMinCostsNew(pTrellisGPU);
            }
            ASSERT_TRUE(sdmTrellisCopyDataCUDA(pTrellisGPU, &trellisCPU));

            EXPECT_NEAR(trellisCPU.minCost, minExpect, c_tolerance);
        }
    }

    sdmTrellisFreeStatesCUDA(pTrellisGPU);

}

//-------------------------------------------------------------------------------------------

TEST_F(SDMTrellisCUDATest, minCostOld)
{
    sdmTrellisTestMinCost(true);
}

//-------------------------------------------------------------------------------------------

TEST_F(SDMTrellisCUDATest, minCostNew_SM80)
{
    sdmTrellisTestMinCost(false);
}

//-------------------------------------------------------------------------------------------

TEST_F(SDMTrellisCUDATest, timeSpeedOfSortAlgorithms)
{
    constexpr int noCombos = 10;
    constexpr int noIters = 5000;

    common::Random *rand = common::Random::instance();
    rand->seed(1);

    double tMerge = 0.0, tRank = 0.0;

    SMDTrellisDataCUDA trellisCPU;
    memset(&trellisCPU, 0, sizeof(SMDTrellisDataCUDA));
    SMDTrellisDataCUDA *pTrellisGPU = sdmTrellisAllocateCUDA();

    for(int combo = 0; combo < noCombos; combo++)
    {
		for(int idx = 0; idx < 64; idx++)
		{
			trellisCPU.cost[idx] = static_cast<float>(rand->randomReal2());
		}
		sdmTrellisCopyDataCUDA(&trellisCPU, pTrellisGPU);

        {
            common::TimeStamp sT = common::TimeStamp::now();
            testSDMTrellisRunMergeSort(pTrellisGPU, noIters);
            sdmTrellisCopyDataCUDA(pTrellisGPU, &trellisCPU);
            common::TimeStamp eT = common::TimeStamp::now();
            common::TimeStamp diffT = eT - sT;
            if(combo >= 1)
            {
                tMerge += static_cast<double>(diffT);
            }
        }
        
        {
            common::TimeStamp sT = common::TimeStamp::now();
            testSDMTrellisRunRankSort(pTrellisGPU, noIters);
            sdmTrellisCopyDataCUDA(pTrellisGPU, &trellisCPU);
			common::TimeStamp eT = common::TimeStamp::now();
			common::TimeStamp diffT = eT - sT;
            if(combo >= 1)
            {
                tRank += static_cast<double>(diffT);
            }
        }

        fprintf(stdout, "Merge time=%.6f, Rank time=%.6f\n", tMerge, tRank);
    }

    sdmTrellisFreeStatesCUDA(pTrellisGPU);
}

//-------------------------------------------------------------------------------------------
