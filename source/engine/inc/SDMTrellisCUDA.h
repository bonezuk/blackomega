//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SDMTRELLISCUDA_H
#define __OMEGA_ENGINE_SDMTRELLISCUDA_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/OmegaCUDACommon.h"
#include "engine/inc/SDMTrellis.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

typedef struct s_SDMTrellisDataCUDA
{
    // row-major
    float states[2][2 * c_maxNoSDMTrellisPaths][8];
    float cost[2 * c_maxNoSDMTrellisPaths];
    // row-major
    float filterG[8][8];
    // row-major
    float filterA[8][8];
    uint32_t path[2 * c_maxNoSDMTrellisPaths];
    int noCandidates;
    int order;
    int rate;
    float gain;
} SDMTrellisDataCUDA;

//-------------------------------------------------------------------------------------------

constexpr int c_SDMTrellisMaxNoBlocks = 1024;

typedef struct s_SDMTrellisBlockCUDA
{
    /* The sample index position where the trellis starts, this includes the warmup. */
    int start;
    /* The sample index position in the trellis from which the output begins, after warmpup. */
    int from;
    /* The sample index position where the trellis block ends. */
    int to;
} SDMTrellisBlockCUDA;

//-------------------------------------------------------------------------------------------

typedef struct s_SDMTrellisBlockDataCUDA
{
    /* In the block algorithm the state, path and cost inside trellis is used to initialise
       the first trellis in the block. */
    SDMTrellisDataCUDA trellis;
    /* The state of the last trellis in a given block is written back into lastTrellis.
       via sdmCUDATrellisCarryOverLastTrellisToNextBlock. */
    SDMTrellisDataCUDA lastTrellis;
    /* The preferred number of samples per trellis block. */
    int prefBlockSamples;
    /* The number of overlapping samples in between each block. */
    int overlay;
    /* The number of samples to process before trellis is stable and create output. */
    int warmup;
    /* Array of block partition information */
    SDMTrellisBlockCUDA block[c_SDMTrellisMaxNoBlocks];
    /* Number of blocks to be processed = blockDim.x of CUDA run */
    int noBlocks;
} SDMTrellisBlockDataCUDA;

//-------------------------------------------------------------------------------------------

/* The output is kept separate from the block data */
typedef struct s_SDMTrellisBlockOutputCUDA
{
    /* The number of allocated bytes in each uint8_t array. */
    int allocated;
    uint8_t *data[c_SDMTrellisMaxNoBlocks];
} SDMTrellisBlockOutputCUDA;

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT void sdmTrellisFreeStatesCUDA(SDMTrellisDataCUDA *states);
ENGINE_EXPORT SDMTrellisDataCUDA *sdmTrellisAllocateCUDA();
ENGINE_EXPORT bool sdmTrellisCopyDataCUDA(SDMTrellisDataCUDA *pSrc, SDMTrellisDataCUDA *pDest);
ENGINE_EXPORT bool initSDMTrellisCUDA(int dsdRate, int order, bool isClans, SDMTrellisDataCUDA *pData);
ENGINE_EXPORT bool initSDMTrellisBlockCUDA(int dsdRate, int order, bool isClans, SDMTrellisBlockDataCUDA *pData);

ENGINE_EXPORT bool sdmCUDATrellisRunSingle(SDMTrellisDataCUDA *pTrellis, const float *inSamples, uint8_t *out, int noSamples, bool isLSB, bool isLatencyOutput);

ENGINE_EXPORT bool sdmCUDATrellisRun(SDMTrellisDataCUDA *pTrellis, const float *inSamples, uint8_t *out, int noSamples, bool isLSB, int noTrellisStreams);

ENGINE_EXPORT int sdmTrellisCUDAQueryBlockDimensions(int noChannels);
ENGINE_EXPORT void sdmTrellisCUDACalcBlockSize(SDMTrellisBlockDataCUDA *pBTrellis, int noSamples);
ENGINE_EXPORT int sdmTrellisCUDABlockOutputSize(SDMTrellisBlockDataCUDA *pBTrellis, int idx);

ENGINE_EXPORT void freeSDMTrellisCUDABlockOutput(SDMTrellisBlockOutputCUDA *pOutput);
ENGINE_EXPORT SDMTrellisBlockOutputCUDA *allocateSDMTrellisCUDABlockOuput(SDMTrellisBlockDataCUDA *pBTrellis, bool isDevice);
ENGINE_EXPORT bool copySDMTrellisCUDABlockOutput(SDMTrellisBlockOutputCUDA *src, SDMTrellisBlockOutputCUDA *dest, int noBlocks);

ENGINE_EXPORT void freeSDMTrellisCUDABlockData(SDMTrellisBlockDataCUDA *pBTrellis);
ENGINE_EXPORT SDMTrellisBlockDataCUDA *allocateSDMTrellisCUDABlockData();
ENGINE_EXPORT void copySDMTrellisCUDABlockData(const SDMTrellisBlockDataCUDA *src, SDMTrellisBlockDataCUDA *dest);

//-------------------------------------------------------------------------------------------
// Unit test API
//-------------------------------------------------------------------------------------------

ENGINE_EXPORT void testSDMTrellisCalcStateAndCostCUDATF32(SDMTrellisDataCUDA *pTrellis, float xSample);
ENGINE_EXPORT void testSDMTrellisCalcStateAndCostCUDAFP32(SDMTrellisDataCUDA *pTrellis, float xSample);
ENGINE_EXPORT void testSDMTrellisRankSortCosts(SDMTrellisDataCUDA *pTrellis);
ENGINE_EXPORT void testSDMTrellisRankSortCostsWithBlanks(SDMTrellisDataCUDA *pTrellis);
ENGINE_EXPORT void testSDMTrellisRankSortPath(SDMTrellisDataCUDA *pTrellis);
ENGINE_EXPORT void testSDMTrellisMinIndex(SDMTrellisDataCUDA *pTrellis);
ENGINE_EXPORT void testSDMTrellisPruneStatePairs(SDMTrellisDataCUDA *pTrellis);
ENGINE_EXPORT void testSDMTrellisFindNoCandidates(SDMTrellisDataCUDA *pTrellis);
ENGINE_EXPORT void testSDMTrellisVibertiStep(SDMTrellisDataCUDA *pTrellis);
ENGINE_EXPORT void testSDMTrellisIntegratorInvariant(SDMTrellisDataCUDA *pTrellis);
ENGINE_EXPORT double *testSDMTrellisFirstIntegrator(const SDMTrellisBlockDataCUDA *pTrellis, const float *pcm, int noSamples, int noBlocks);
ENGINE_EXPORT void testSDMTrellisRun(SDMTrellisBlockDataCUDA *pBlock,  const float *in, SDMTrellisBlockOutputCUDA *pOutput, int noBlocks);

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omege
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
