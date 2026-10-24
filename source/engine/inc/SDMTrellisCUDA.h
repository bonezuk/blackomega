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
    int path[2 * c_maxNoSDMTrellisPaths];
    int noCandidates;
    float minCost;
} SMDTrellisDataCUDA;

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT void sdmTrellisFreeStatesCUDA(SMDTrellisDataCUDA *states);
ENGINE_EXPORT SMDTrellisDataCUDA *sdmTrellisAllocateCUDA();
ENGINE_EXPORT bool sdmTrellisCopyDataCUDA(SMDTrellisDataCUDA *pSrc, SMDTrellisDataCUDA *pDest);
ENGINE_EXPORT bool initSDMTrellisFilterCUDA(int dsdRate, bool isClans, SMDTrellisDataCUDA *pData);

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT void testSDMTrellisCalcStateAndCostCUDATF32(SMDTrellisDataCUDA *pTrellis, float xSample);
ENGINE_EXPORT void testSDMTrellisCalcStateAndCostCUDAFP32(SMDTrellisDataCUDA *pTrellis, float xSample);
ENGINE_EXPORT void testSDMTrellisSortCosts(SMDTrellisDataCUDA *pTrellis);
ENGINE_EXPORT void testSDMTrellisMinCostsOld(SMDTrellisDataCUDA *pTrellis);
ENGINE_EXPORT void testSDMTrellisMinCostsNew(SMDTrellisDataCUDA *pTrellis);

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omege
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
