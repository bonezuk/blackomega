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

typedef struct s_SMDTrellisDataCUDA
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
} SMDTrellisDataCUDA;

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT void sdmTrellisFreeStatesCUDA(SMDTrellisDataCUDA *states);
ENGINE_EXPORT SMDTrellisDataCUDA *sdmTrellisAllocateCUDA();
ENGINE_EXPORT bool sdmTrellisCopyDataCUDA(SMDTrellisDataCUDA *pSrc, SMDTrellisDataCUDA *pDest);
ENGINE_EXPORT bool initSDMTrellisCUDA(int dsdRate, int order, bool isClans, SMDTrellisDataCUDA *pData);

ENGINE_EXPORT bool sdmCUDATrellisRunSingle(SMDTrellisDataCUDA *pTrellis, const float *inSamples, uint8_t *out, int noSamples, bool isLSB, bool isLatencyOutput);

//-------------------------------------------------------------------------------------------
// Unit test API
//-------------------------------------------------------------------------------------------

ENGINE_EXPORT void testSDMTrellisCalcStateAndCostCUDATF32(SMDTrellisDataCUDA *pTrellis, float xSample);
ENGINE_EXPORT void testSDMTrellisCalcStateAndCostCUDAFP32(SMDTrellisDataCUDA *pTrellis, float xSample);
ENGINE_EXPORT void testSDMTrellisRankSortCosts(SMDTrellisDataCUDA *pTrellis);
ENGINE_EXPORT void testSDMTrellisRankSortCostsWithBlanks(SMDTrellisDataCUDA *pTrellis);
ENGINE_EXPORT void testSDMTrellisRankSortPath(SMDTrellisDataCUDA *pTrellis);
ENGINE_EXPORT void testSDMTrellisMinIndex(SMDTrellisDataCUDA *pTrellis);
ENGINE_EXPORT void testSDMTrellisPruneStatePairs(SMDTrellisDataCUDA *pTrellis);
ENGINE_EXPORT void testSDMTrellisFindNoCandidates(SMDTrellisDataCUDA *pTrellis);
ENGINE_EXPORT void testSDMTrellisVibertiStep(SMDTrellisDataCUDA *pTrellis);

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omege
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
