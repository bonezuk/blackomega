#include <cuda_runtime.h>
#include <math_constants.h>
#include <cute/tensor.hpp>
#include <cute/layout.hpp>
#include <cute/atom/mma_traits_sm80.hpp>

#include "cutlass/util/GPU_Clock.hpp"

#include "common/inc/CommonTypes.h"
#include "engine/inc/FFTRadix2Cuda.h"
#include "engine/inc/SDMTrellisCUDA.h"

using namespace cute;

//-------------------------------------------------------------------------------------------
#if defined(__CUDA_ARCH__) && (__CUDA_ARCH__ >= 800)
#define __OMEGA_CUDA_SM80
#endif
//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

using Traits = MMA_Traits<SM80_16x8x8_F32TF32TF32F32_TN>;

//-------------------------------------------------------------------------------------------

__device__ void kSDMTrellisLoadPaths(SMDTrellisDataCUDA *pTrellis, uint32* __restrict__ path)
{
    if(threadIdx.x < 32)
    {
        path[threadIdx.x] = states->path[threadIdx.x];
    }
}

//-------------------------------------------------------------------------------------------

__device__ void kSDMTrellisLoadCosts(SMDTrellisDataCUDA *pTrellis, uint32* __restrict__ cost)
{
    if(threadIdx.x < 32)
    {
        cost[threadIdx.x] = states->cost[threadIdx.x];
    }
}

//-------------------------------------------------------------------------------------------

__device__ void kSMDTrellisLoadStates(SMDTrellisDataCUDA *pTrellis, float* __restrict__ states)
{
    float *pMatStateCurr = &(pTrellis->states[0][0][0]);
    int idxS = threadIdx.x << 2;
    #pragma unroll
    for(int i = 0; i < 4; i++)
    {
        states[idxS + i] = pMatStateCurr[idxS + i];
    }   
}

//-------------------------------------------------------------------------------------------
#if defined(__OMEGA_CUDA_SM80)
//-------------------------------------------------------------------------------------------

__device__ __forceinline__ uint32_t kFloatToTF32(float x)
{
    uint32_t r;
    asm("cvt.rna.tf32.f32 %0, %1;\n" : "=r"(r) : "f"(x));
    return r;
}

//-------------------------------------------------------------------------------------------

__device__ __forceinline__ void kSplitTF32(float x, uint32_t& hi, uint32_t& lo)
{
    hi = kFloatToTF32(x);
    lo = kFloatToTF32(x - __uint_as_float(hi));
}

//-------------------------------------------------------------------------------------------

__device__  __forceinline__ void kMatrixMultiplyTF32_16x8x8(
    float& d0, float& d1, float& d2, float& d3,
    uint32_t const& a0, uint32_t const& a1, uint32_t const& a2, uint32_t const& a3,
    uint32_t const& b0, uint32_t const& b1,
    float const& c0, float const& c1, float const& c2, float const& c3)
{
    asm volatile("mma.sync.aligned.m16n8k8.row.col.f32.tf32.tf32.f32 "
        "{%0, %1, %2, %3}, "
        "{%4, %5, %6, %7}, "
        "{%8, %9}, "
        "{%10, %11, %12, %13};\n"
        : "=f"(d0), "=f"(d1), "=f"(d2), "=f"(d3)
        :  "r"(a0),  "r"(a1),  "r"(a2),  "r"(a3),
           "r"(b0),  "r"(b1),
           "f"(c0),  "f"(c1),  "f"(c2),  "f"(c3)
    );
}

//-------------------------------------------------------------------------------------------

__device__ void kSDMTrellisSplitTF32Matrix8x8(const float* __restrict__ inMatrix,
                                              uint32_t* __restrict__ highM,
                                              uint32_t* __restrict__ lowM,
                                              int noRows)
{
    for(int idx = 0; idx < (noRows << 3); idx += 32)
    {
        int i = idx + (threadIdx.x & 0x1f);
        kSplitTF32(inMatrix[i], highM[i], lowM[i]);
    }
    __syncwarp();
}

//-------------------------------------------------------------------------------------------

__device__ __forceinline__ void kSDMTrellisMatrixCalcFP32_16x8x8(const float* __restrict__ A, 
                                                 const uint32_t* __restrict__ hiB, const uint32_t* __restrict__ loB,
                                                 float* __restrict__ D, 
                                                 const int* __restrict__ idxA, const int* __restrict__ idxB, const int* __restrict__ idxC,
                                                 const float xSample)
{
    __shared__ uint32_t loA[16 * 8];
    __shared__ uint32_t hiA[16 * 8];

    float acc[4];
    float cor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    for(int i = 0; i < 4; i++)
    {
        acc[i] = (!(idxC[i] & 0x7)) ? xSample : 0.0f;
    }

    kSDMTrellisSplitTF32Matrix8x8(A, hiA, loA, 16);

    kMatrixMultiplyTF32_16x8x8(cor[0], cor[1], cor[2], cor[3],
                        loA[idxA[0]], loA[idxA[1]], loA[idxA[2]], loA[idxA[3]],
                        hiB[idxB[0]], hiB[idxB[1]],
                        cor[0], cor[1], cor[2], cor[3]);

    kMatrixMultiplyTF32_16x8x8(cor[0], cor[1], cor[2], cor[3],
                        hiA[idxA[0]], hiA[idxA[1]], hiA[idxA[2]], hiA[idxA[3]],
                        loB[idxB[0]], loB[idxB[1]],
                        cor[0], cor[1], cor[2], cor[3]);

    kMatrixMultiplyTF32_16x8x8(acc[0], acc[1], acc[2], acc[3],
                        hiA[idxA[0]], hiA[idxA[1]], hiA[idxA[2]], hiA[idxA[3]],
                        hiB[idxB[0]], hiB[idxB[1]],
                        acc[0], acc[1], acc[2], acc[3]);

    for(int i = 0; i < 4; i++)
    {
        D[idxC[i]] = acc[i] + cor[i];
    }
}

//-------------------------------------------------------------------------------------------

__device__ void kSDMTrellisMMALayout(int* __restrict__ idxA, int* __restrict__ idxB, int* __restrict__ idxC)
{
    // A: M X K
    auto layoutRowA = Layout<Shape<_16, _8>, Stride<_8, _1>>{};
    auto tvA = composition(layoutRowA, Traits::ALayout{});
    // B: N x K
    auto layoutRowB = Layout<Shape<_8, _8>, Stride<_1, _8>>{}; // (n, k) : N x K
    auto tvB = composition(layoutRowB, Traits::BLayout{});
    // C and D : M x N
    auto layoutRowC = Layout<Shape<_16, _8>, Stride<_8, _1>>{};
    auto tvC = composition(layoutRowC, Traits::CLayout{});

    for(int i = 0; i < 4; i++)
    {
        idxA[i] = tvA(threadIdx.x, i);
        idxC[i] = tvC(threadIdx.x, i);
    }
    for(int i = 0; i < 2; i++)
    {
        idxB[i] = tvB(threadIdx.x, i);
    }
}

//-------------------------------------------------------------------------------------------

__device__ void kSDMTrellisNextStateAndCost(float* __restrict__ pState, 
                                            float* __restrict__ pCost,
                                            const float* __restrict__ dMatrix, 
                                            const float* __restrict__ vMatrix, 
                                            float a0)
{

    float* __restrict__ pNextA = pState;
    float* __restrict__ pNextB = &pState[32 * 8];
    int idx = threadIdx.x << 3;

    pNextA[idx] = dMatrix[idx] + 1.0f;
    pNextB[idx] = dMatrix[idx] - 1.0f;
    for(int i = 1; i < 8; i++)
    {
        pNextA[idx + i] = dMatrix[idx + i];
        pNextB[idx + i] = dMatrix[idx + i];
    }

    float vA = vMatrix[idx] + a0;
    vA *= vA;
    pCost[threadIdx.x << 1] += vA;

    float vB = vMatrix[idx] - a0;
    vB *= vB;
    pCost[(threadIdx.x << 1) + 1] += vB;
}

//-------------------------------------------------------------------------------------------

__device__ __forceinline__ void kSDMTrellisCalcStateAndCostFP32(const float* __restrict__ cState,
                 const uint32_t* __restrict__ matLoAFilter, const uint32_t* __restrict__ matHiAFilter,
                 const uint32_t* __restrict__ matLoGFilter, const uint32_t* __restrict__ matHiGFilter,
                 const int* __restrict__ idxA, const int* __restrict__ idxB, const int* __restrict__ idxC,
                 float* __restrict__ dMatrix, float* __restrict__ vMatrix, float* __restrict__ nStateA,
                 const float* __restrict__  cCost, float* __restrict__ nCost,
                 const float a0, float xSample)
{
    bool isLower = (!(threadIdx.x & 0x1));
    const float* __restrict__ pCMatState = (isLower) ? cState : &cState[16 * 8];
    float* __restrict__ pDMatrix = (isLower) ? dMatrix : &dMatrix[16 * 8];
    float* __restrict__ pVMatrix = (isLower) ? vMatrix : &vMatrix[16 * 8];

    kSDMTrellisMatrixCalcFP32_16x8x8(pCMatState, 
            matHiGFilter, matLoGFilter,
            pDMatrix,
            idxA, idxB, idxC,
            xSample);
    __syncwarp();

    kSDMTrellisMatrixCalcFP32_16x8x8(pDMatrix, 
            matHiAFilter, matLoAFilter,
            pVMatrix,
            idxA, idxB, idxC,
            xSample);
    __syncwarp();

    int idxA = (threadIdx.x & 0xfffffffe) << 2;
    nStateA[threadIdx.x] = dMatrix[idxA] + ((isLower) ? 1.0f : -1.0f);

    float v = vMatrix[idxA] + ((isLower) ? a0 : -a0);
    nCost[threadIdx.x] = cCost[threadIdx.x >> 1] + (v * v);

    __syncthreads();
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

__device__ __forceinline__ uint32_t kOrderedIntegerFloat(float f)
{
    uint32_t u = __float_as_uint(f);
    return (u & 0x80000000) ? ~u : (u | 0x80000000);
}

//-------------------------------------------------------------------------------------------

__device__ __forceinline__ float kOrderedFloatFromInteger(uint32_t u)
{
    u = (u & 0x80000000) ? (u & 0x7fffffff) : ~u;
    return __uint_as_float(u);
}

//-------------------------------------------------------------------------------------------

__device__ __forceinline__ void kSDMTrellisRankSortCosts(const float c, int N, int* __restrict__ order)
{
    const int i = threadIdx.x;

    __shared__ __align__(16) uint32_t keys[64];
    const uint32_t ki = (i < N && order[i] != 0xffffffff) ? kOrderedIntegerFloat(c) : 0xffffffff;
    keys[i] = ki;
    __syncthreads();

    const uint4 *k4 = reinterpret_cast<const uint4 *>(keys);
    int rank = 0;
    #pragma unroll
    for(int q = 0; q < 16; q++)
    {
        const uint4 k = k4[q];
        const int j = q << 2;
        rank += (k.x < ki) | ((k.x == ki) & (j + 0 < i));
        rank += (k.y < ki) | ((k.y == ki) & (j + 1 < i));
        rank += (k.z < ki) | ((k.z == ki) & (j + 2 < i));
        rank += (k.w < ki) | ((k.w == ki) & (j + 3 < i));
    }

    if(i < N)
    {
        order[rank] = i;
    }
    __syncthreads();
}

//-------------------------------------------------------------------------------------------

__device__ __forceinline__ void kSDMTrellisRankSortPath(const uint32_t nState, int N, int* __restrict__ order)
{
    const int i = threadIdx.x;

    __shared__ __align__(16) uint32_t keys[64];
    keys[i] = (i < N) ? nState : 0xffffffff;
    __syncthreads();

    const uint4 *k4 = reinterpret_cast<const uint4 *>(keys);
    int rank = 0;
    #pragma unroll
    for(int q = 0; q < 16; q++)
    {
        const uint4 k = k4[q];
        const int j = q << 2;
        rank += (k.x < ki) | ((k.x == ki) & (j + 0 < i));
        rank += (k.y < ki) | ((k.y == ki) & (j + 1 < i));
        rank += (k.z < ki) | ((k.z == ki) & (j + 2 < i));
        rank += (k.w < ki) | ((k.w == ki) & (j + 3 < i));
    }

    if(i < N)
    {
        order[rank] = i;
    }
    __syncthreads();
}

//-------------------------------------------------------------------------------------------

__device__ __forceinline__ int kSDMTrellisFindNoCandidates(const uint32_t *indices, int N)
{
    int lo = 0;
    int hi = N;

    while(lo < hi)
    {
        int mid = lo + ((hi - lo) >> 1);
        if(indices[mid] != 0xffffffff)
        {
            lo = mid + 1;
        }
        else
        {
            hi = mid;
        }
    }
    if(lo > 32)
    {
        lo = 32;
    }
    return lo;
}

//-------------------------------------------------------------------------------------------

__device__ __forceinline__ void kSDMTrellisPruneStatePairs(uint32_t* __restrict__ nStates, uint32_t* __restrict__ indices,
										const float* __restrict__ nCost, int N)
{
    // create ordered index of states
    __shared__ __align__(16) int order[64];
    kSDMTrellisRankSortPath(nStates[i], N, order);

    // populate indices
    indices[threadIdx.x] = (i < N) ? i : 0xffffffff;
        
    // compare for state pairs
    if(i < (N - 1))
    {
        int idxL = order[i];
        int idxR = order[i + 1];
        if(nStates[idxL] == nStates[idxR])
        {
            // remove the state with the highest cost
            int idxDel = (nCost[idxL] <= nCost[idxR]) ? idxR : idxL;
            indices[idxDel] = 0xffffffff;
        }
    }
    __syncthreads();
}

//-------------------------------------------------------------------------------------------

__device__ __forceinline__ int kSDMTrellisVibertiStep(uint32_t* __restrict__ nPath, int N, const uint32_t* __restrict__ cPath, 
                                       const float* __restrict__ nCost, uint32_t* __restrict__ indices, 
                                       uint32_t trellisMask, int outputBit)
{
    const int i = threadIdx.x

    // step path to next from current
    nPath[i] = cPath[i >> 1] | (i & 0x1) ? 1 : 0;
        
    // populate nStates array with next trellis state
    __shared__ __align__(16) uint32_t nStates[64];
    nStates[i] = nPath[i] & trellisMask;
    
    // prune state pairs
    kSDMTrellisPruneStatePairs(nStates, indices, nCost, N);

    // remove indices whose output bit does not corrispond to this stage's output bit.
    int prevIdx = i >> 1;
    int bit = (cPath[prevIdx] & 0x80000000) ? 1 : 0;
    if(bit != outputBit)
    {
        indices[i] = 0xffffffff;
    }
    __syncthreads();
    
    // sort remaining candidates by cost
    kSDMTrellisRankSortCosts(nCost[i], N, indices);

    // return number of new candidates after Viberti step
    return kSDMTrellisFindNoCandidates(indices, N);
}

//-------------------------------------------------------------------------------------------
/* The min index is searched across (32 thread) 2 warps. The first part compares values 
   across the two warps before using the warp level instruction __shfl_xor_sync to sort
   across each warp separately. */
//-------------------------------------------------------------------------------------------

__device__ __forceinline__ int kSDMTrellisMinIndex(const float*  __restrict__ vals, int N)
{
    const int lane = threadIdx.x & 0x1f;
    const int iA = lane;
    const int iB = iA + 32;
    float vA = (iA < N) ? vals[iA] : CUDART_INF_F;
    float vB = (iB < N) ? vals[iB] : CUDART_INF_F;
    float best, idx;
    if(vA < vB)
    {
        best = vA;
        idx = iA;
    }
    else
    {
        best = vB;
        idx = iB;
    }

    #pragma unroll
    for(int mask = 16; mask > 0; mask >>= 1)
    {
        float oV = __shfl_xor_sync(0xffffffff, best, mask);
        int   oI = __shfl_xor_sync(0xffffffff,  idx, mask);
        if(oV < best || (oV == best && oI < idx))
        {
            best = oV;
            idx = oI;
        }
    }
    return idx;
}

//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisRun(SMDTrellisDataCUDA *pTrellis, const float *inSamples, uint8_t *out, int noSamples, bool isLSB, bool isLatencyOutput)
{
    // 64 * 2 * 4 = 512 : S 512
    __shared__ __align__(16) uint32_t matLoAFilter[64], matHiAFilter[64]; 
    // 64 * 2 * 4 = 512 : S 1024
    __shared__ __align__(16) uint32_t matLoGFilter[64], matHiGFilter[64];
    float *pAFilter = &(pTrellis->filterA[0][0]);
    float *pGFilter = &(pTrellis->filterG[0][0]);
    kSDMTrellisSplitTF32Matrix8x8(pAFilter, matHiAFilter, matLoAFilter, 8);
    kSDMTrellisSplitTF32Matrix8x8(pGFilter, matHiGFilter, matLoGFilter, 8);
    float a0 = pTrellis->filterA[0][0];

    int idxA[4], idxB[2], idxC[4];
    kSDMTrellisMMALayout(idxA, idxB, idxC);

    // 32 * 4 = 128 : S 1152
    __shared__ __align__(16) uint32_t cPath[32];
    // 64 * 4 = 256 : S 1408
    __shared__ __align__(16) uint32_t nPath[64];
    kSDMTrellisLoadPaths(pTrellis, cPath);

    /* cState - at the start of each step holds the current filter states. 
       8 states per candidate with a maximum of 32 candidates.
       The calculation of the next set of filter states is placed into dMatrix.
       The creation of 2 (Ai,Bi) new candidates from 1 path has each of the 2 new filter states:
         - dMatrix[32][1 - 7] are both shared by each of the 2 new candidates
         - nState[0 -31] state of Ai = dMatrix[i][0] + 1.0
         - nState[32-63] state of Bi = dMatrix[i][0] - 1.0
       The vMatrix holds the accumulation of each input path that is used to calculate the cost
       of the given step.
       cCost holds the current cost of each path. For each path i
       nCost[ 0-31] = nCost[i] + (vMatrix[i][0] + a0)^2
       nCost[32-64] = nCost[i] + (vMatrix[i][0] - a0)^2
    */

    // 32 * 8 * 4 = 1024 : S 2432
    __shared__ __align__(16) float dMatrix[32 * 8];
    // 32 * 8 * 4 = 1024 : S 3456
    __shared__ __align__(16) float vMatrix[32 * 8];
    // 32 * 8 * 4 = 1024 : S 4480
    __shared__ __align__(16) float cState[32 * 8];
    // 32 * 4 = 256 : S 4736
    __shared__ __align__(16) float nStateA[64];
    // 32 * 4 = 128 : S 4864
    __shared__ __align__(16) float cCost[32];
    // 64 * 4 = 256 : S 5120
    __shared__ __align__(16) float nCost[64];    
    // 64 * 4 = 256 : S 5376
    __shared__ __align__(16) uint32_t indices[64];

    kSMDTrellisLoadStates(pTrellis, cState);
    kSDMTrellisLoadCosts(pTrellis, cCost);

    uint32_t trellisMask = (1 << pTrellis->order) - 1;
    int noCandidates = pTrellis->noCandidates;
    __syncthreads();

	constexpr uint8_t maskMSB[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
	constexpr uint8_t maskLSB[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
    constexpr uint8_t *mask = (isLSB) ? maskMSB : maskLSB;
    int noOutSamples = 0;
    uint8_t dsd = 0;

    const int i = threadIdx.x;

    for(int sampleIdx = 0; sampleIdx < noSamples; sampleIdx++)
    {
        float sample = inSamples[sampleIdx];

        kSDMTrellisCalcStateAndCostFP32(cState,
            matLoAFilter, matHiAFilter,
            matLoGFilter, matHiGFilter,
            idxA, idxB, idxC,
            dMatrix, vMatrix, nStateA,
            cCost, nCost,
            a0, sample);
        
        // Each candidate from the current step produces 2 potential candidates
        noCandidates <<= 1;

        // Find the potential candiate with the least cost
        int minCostIndex = kSDMTrellisMinIndex(nCost, noCandidates);
        float minCost = nCost[minCostIndex];

        // Find the output bit for this step at latency=32 based upon the minimum cost
        int iA = minCostIndex >> 1;
        int outputBit = (cCost[iA] & 0x80000000) ? 1 : 0;

        // Perform Viberti and get new candidates.
        noCandidates = kSDMTrellisVibertiStep(nPath, N, cPath, nCost, indices, trellisMask, outputBit);

        // Copy state and cost of next into current for next step
        if(i < noCandidates)
        {
            int sIdx = indices[i];
            int dIdx = i << 3;
            cCost[i] = nCost[sIdx] - minCost;
            cPath[i] = nPath[sIdx];
            cState[dIdx] = nStateA[sIdx];
            sIdx = (sIdx & 0xfffffffe) << 2;
            for(int j = 1; j < 8; j++)
            {
                cState[dIdx + j] = dMatrix[sIdx + j];
            }
        }

        if(i == 32 && (isLatencyOutput || sampleIdx >= 32))
        {
            if(outputBit)
            {
                dsd |= mask[noOutSamples & 0x7];
            }
            noOutSamples++;
            if(!(noOutSamples & 0x7))
            {
                out[noOutSamples >> 3] = dsd;
                dsd = 0;
            }
        }
        __syncthreads();
    }
}

//-------------------------------------------------------------------------------------------
// Supporting kernels for unit testing
//-------------------------------------------------------------------------------------------
#if defined(__OMEGA_CUDA_SM80)
//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestCalcStateAndCostFP32(SMDTrellisDataCUDA *pTrellis, float xSample)
{
    __shared__ __align__(16) uint32_t matLoAFilter[64], matHiAFilter[64];
    __shared__ __align__(16) uint32_t matLoGFilter[64], matHiGFilter[64];
    float *pAFilter = &(pTrellis->filterA[0][0]);
    float *pGFilter = &(pTrellis->filterG[0][0]);
    kSDMTrellisSplitTF32Matrix8x8(pAFilter, matHiAFilter, matLoAFilter, 8);
    kSDMTrellisSplitTF32Matrix8x8(pGFilter, matHiGFilter, matLoGFilter, 8);
    float a0 = pTrellis->filterA[0][0];

    int idxA[4], idxB[2], idxC[4];
    kSDMTrellisMMALayout(idxA, idxB, idxC);

    __shared__ __align__(16) float dMatrix[32 * 8];
    __shared__ __align__(16) float vMatrix[32 * 8];
    __shared__ __align__(16) float cState[32 * 8];
    __shared__ __align__(16) float nStateA[64];
    __shared__ __align__(16) float cCost[32];
    __shared__ __align__(16) float nCost[64];    
   
    kSMDTrellisLoadStates(pTrellis, cState);
    kSDMTrellisLoadCosts(pTrellis, cCost);
    __syncthreads();

    kSDMTrellisCalcStateAndCostFP32(cState,
        matLoAFilter, matHiAFilter,
        matLoGFilter, matHiGFilter,
        idxA, idxB, idxC,
        dMatrix, vMatrix, nStateA,
        cCost, nCost,
        a0, sample);
    
    int dIdx = (threadIdx.x & 0xfffffffe) << 2;
    pTrellis->states[1][threadIdx.x][0] = nStateA[threadIdx.x];
    for(int i = 1; i < 8; i++)
    {
        pTrellis->states[1][threadIdx.x][i] = nStateA[dIdx + i];
    }

    pTrellis->cost[threadIdx.x] = nCost[threadIdx.x];
}

//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestCalcStateAndCostTF32(SMDTrellisDataCUDA *pTrellis, float xSample)
{
    __shared__ uint32_t matrixAFilter[64];
    __shared__ uint32_t matrixGFilter[64];
    float *pAFilter = &(pTrellis->filterA[0][0]);
    float *pGFilter = &(pTrellis->filterG[0][0]);
    int idx;
    
    idx = threadIdx.x << 1;
    for(int i = 0; i < 2; i++)
    {
        matrixAFilter[idx + i] = kFloatToTF32(pAFilter[idx + i]);
        matrixGFilter[idx + i] = kFloatToTF32(pGFilter[idx + i]);
    }

    __shared__ uint32_t mStateCurr[256]; // 32x8
    __shared__ float dMatrix[256]; // 32x8
    __shared__ float cMatrix[128]; // 16x8
    __shared__ float vMatrix[256]; // 32x8
    float *pStateCurr = &(pTrellis->states[0][0][0]);

    idx = threadIdx.x << 3;
    for(int i = 0; i < 8; i++)
    {
        mStateCurr[idx + i] = kFloatToTF32(pStateCurr[idx + i]);
        dMatrix[idx + i] = 0.0f;
        vMatrix[idx + i] = 0.0f;
    }
    idx = threadIdx.x << 2;
    cMatrix[idx] = (!(threadIdx.x & 0x1)) ? xSample : 0.0f;
    for(int i = 1; i < 4; i++)
    {
        cMatrix[idx+i] = 0.0f;
    }

    __syncwarp();
    
    // A: M X K
    auto layoutRowA = Layout<Shape<_16, _8>, Stride<_8, _1>>{};
    auto tvA = composition(layoutRowA, Traits::ALayout{});
    // B: N x K
    auto layoutRowB = Layout<Shape<_8, _8>, Stride<_1, _8>>{}; // (n, k) : N x K
    auto tvB = composition(layoutRowB, Traits::BLayout{});
    // C and D : M x N
    auto layoutRowC = Layout<Shape<_16, _8>, Stride<_8, _1>>{};
    auto tvC = composition(layoutRowC, Traits::CLayout{});

    idx = threadIdx.x;
    kMatrixMultiplyTF32_16x8x8(dMatrix[tvC(idx, 0)], dMatrix[tvC(idx, 1)], dMatrix[tvC(idx, 2)], dMatrix[tvC(idx, 3)],
                   mStateCurr[tvA(idx, 0)], mStateCurr[tvA(idx, 1)], mStateCurr[tvA(idx, 2)], mStateCurr[tvA(idx, 3)], 
                   matrixGFilter[tvB(idx, 0)], matrixGFilter[tvB(idx, 1)],
                   cMatrix[tvC(idx, 0)], cMatrix[tvC(idx, 1)],cMatrix[tvC(idx, 2)],cMatrix[tvC(idx, 3)]);
    
    float *dMatrixP2 = &dMatrix[128];
    uint32_t *mStateCurrP2 = &mStateCurr[128];
    kMatrixMultiplyTF32_16x8x8(dMatrixP2[tvC(idx, 0)], dMatrixP2[tvC(idx, 1)], dMatrixP2[tvC(idx, 2)], dMatrixP2[tvC(idx, 3)],
                   mStateCurrP2[tvA(idx, 0)], mStateCurrP2[tvA(idx, 1)], mStateCurrP2[tvA(idx, 2)], mStateCurrP2[tvA(idx, 3)], 
                   matrixGFilter[tvB(idx, 0)], matrixGFilter[tvB(idx, 1)],
                   cMatrix[tvC(idx, 0)], cMatrix[tvC(idx, 1)],cMatrix[tvC(idx, 2)],cMatrix[tvC(idx, 3)]);

                   
    __syncwarp();

    uint32_t *dMatTF32 = reinterpret_cast<uint32_t *>(dMatrix);
    kMatrixMultiplyTF32_16x8x8(vMatrix[tvC(idx, 0)], vMatrix[tvC(idx, 1)], vMatrix[tvC(idx, 2)], vMatrix[tvC(idx, 3)],
                   dMatTF32[tvA(idx, 0)], dMatTF32[tvA(idx, 1)], dMatTF32[tvA(idx, 2)], dMatTF32[tvA(idx, 3)],
                   matrixAFilter[tvB(idx, 0)], matrixAFilter[tvB(idx, 1)],
                   cMatrix[tvC(idx, 0)], cMatrix[tvC(idx, 1)],cMatrix[tvC(idx, 2)],cMatrix[tvC(idx, 3)]);
    
    float *vMatrixP2 = &vMatrix[128];
    dMatTF32 = reinterpret_cast<uint32_t *>(&dMatrix[128]);
    kMatrixMultiplyTF32_16x8x8(vMatrixP2[tvC(idx, 0)], vMatrixP2[tvC(idx, 1)], vMatrixP2[tvC(idx, 2)], vMatrixP2[tvC(idx, 3)],
                   dMatTF32[tvA(idx, 0)], dMatTF32[tvA(idx, 1)], dMatTF32[tvA(idx, 2)], dMatTF32[tvA(idx, 3)],
                   matrixAFilter[tvB(idx, 0)], matrixAFilter[tvB(idx, 1)],
                   cMatrix[tvC(idx, 0)], cMatrix[tvC(idx, 1)],cMatrix[tvC(idx, 2)],cMatrix[tvC(idx, 3)]);

    __syncwarp();

    kSDMTrellisNextStateAndCost(&(pTrellis->states[1][0][0]), pTrellis->cost, dMatrix, vMatrix, pTrellis->filterA[0][0]);
}

//-------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestCalcStateAndCostFP32(SMDTrellisDataCUDA *pTrellis, float xSample)
{}

//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestCalcStateAndCostTF32(SMDTrellisDataCUDA *pTrellis, float xSample)
{}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestRankSortPath(SMDTrellisDataCUDA *pTrellis)
{
    const int i = threadIdx.x;
    uint32_t state = pTrellis->path[i];
    int N = pTrellis->noCandidates;
    __shared__ __align__(16) order[64];
    kSDMTrellisRankSortPath(state, N, order);
    if(i < N)
    {
        pTrellis->path[i] = pTrellis->path[order[i]];
    }
}

//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestFindNoCandidates(SMDTrellisDataCUDA *pTrellis)
{
	const int i = threadIdx.x;
	__shared__ __align__(16) uint32_t indices[64];
	indices[i] = pTrellis->path[i];
	pTrellis->path[i] = kSDMTrellisFindNoCandidates(indices, pTrellis->noCandidates);
}

//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestPruneStatePairs(SMDTrellisDataCUDA *pTrellis)
{
    __shared__ float cost[64];
    __shared__ uint32_t states[64]
    __shared__ int indices[64];
    int N = pTrellis->noCandidates;

    if(threadIdx.x < N)
    {
        cost[threadIdx.x] = pTrellis->cost[threadIdx.x];
        states[threadIdx.x] = pTrellis->path[threadIdx.x];
    }
    __syncthreads();
    
    kSDMTrellisPruneStatePairs(states, indices, cost, N);

    if(threadIdx.x < N)
    {
        pTrellis->path[threadIdx.x] = indices[threadIdx.x];
    }

}

//-------------------------------------------------------------------------------------------

__global__ void kTestSDMTrellisRankSortCosts(SMDTrellisDataCUDA *pTrellis)
{
    __shared__ float cost[64];
    __shared__ int order[64];
    int N = pTrellis->noCandidates;

    if(threadIdx.x < N)
    {
        cost[threadIdx.x] = pTrellis->cost[threadIdx.x];
        order[threadIdx.x] = threadIdx.x;
    }
    __syncthreads();

    kSDMTrellisRankSortCosts(cost[threadIdx.x], N, order);

    if(threadIdx.x < N)
    {
        pTrellis->path[threadIdx.x] = order[threadIdx.x];
    }
}

//-------------------------------------------------------------------------------------------

__global__ void kTestSDMTrellisRankSortCostsWithBlanks(SMDTrellisDataCUDA *pTrellis)
{
    __shared__ float cost[64];
    __shared__ int order[64];
    int N = pTrellis->noCandidates;

    if(threadIdx.x < N)
    {
        cost[threadIdx.x] = pTrellis->cost[threadIdx.x];
        order[threadIdx.x] = pTrellis->path[threadIdx.x];
    }
    __syncthreads();

    kSDMTrellisRankSortCosts(cost[threadIdx.x], N, order);

    if(threadIdx.x < N)
    {
        pTrellis->path[threadIdx.x] = order[threadIdx.x];
    }
}

//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestMinIndex(SMDTrellisDataCUDA *pTrellis)
{
    int N = pTrellis->noCandidates;
    __shared__ __align__(16) flost cost[64];
    cost[threadIdx.x] = pTrellis->cost[threadIdx.x];
    __syncthreads();
    int minIdx = kSDMTrellisMinIndex(cost, N);
    pTrellis->path[threadIdx.x] = static_cast<uint32_t>(minIdx);
}

//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestVibertiStep(SMDTrellisDataCUDA *pTrellis)
{
    const int i = threadIdx.x;
    __shared__ uint32_t cPath[32], nPath[64];
    __shared__ float nCost[64];
    __shared__ uint32_t indices[64];
    if(i < 32)
    {
        cPath[i] = pTrellis->path[i];
        nCost[i] = pTrellis->cost[i];
    }
    int N = pTrellis->noCandidates;
    int trellisMask = (1 << pTrellis->order) - 1;
    __syncthreads();

    N = kSDMTrellisVibertiStep(nPath, N, cPath, nCost, indices, trellisMask, 0);
    if(i < N)
    {
        int sIdx = indices[i];
        pTrellis->cost[i] = nCost[sIdx];
        pTrellis->path[i] = nPath[sIdx];
    }
    if(!i)
    {
        pTrellis->noCandidates = N;
    }
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisCalcStateAndCostCUDATF32(SMDTrellisDataCUDA *pTrellis, float xSample)
{
    kSDMTrellisTestCalcStateAndCostTF32<<<1, 32>>>(pTrellis, xSample);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisCalcStateAndCostCUDAFP32(SMDTrellisDataCUDA *pTrellis, float xSample)
{
    kSDMTrellisTestCalcStateAndCostFP32<<<1, 64>>>(pTrellis, xSample);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisRankSortCosts(SMDTrellisDataCUDA *pTrellis)
{
    kTestSDMTrellisRankSortCosts<<<1, 64>>>(pTrellis);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisMinIndex(SMDTrellisDataCUDA *pTrellis)
{
    kSDMTrellisTestMinIndex<<<1, 64>>>(pTrellis);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisRankSortPath(SMDTrellisDataCUDA *pTrellis)
{
    kSDMTrellisTestRankSortPath<<<1, 64>>>(pTrellis);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisRankSortPathWithBlanks(SMDTrellisDataCUDA *pTrellis)
{
    kTestSDMTrellisRankSortCostsWithBlanks<<<1, 64>>>(pTrellis);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisVibertiStep(SMDTrellisDataCUDA *pTrellis)
{
    kSDMTrellisTestVibertiStep<<<1, 64>>>(pTrellis);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisPruneStatePairs(SMDTrellisDataCUDA *pTrellis)
{
	kSDMTrellisTestPruneStatePairs<<<1, 64>>>(pTrellis);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisFindNoCandidates(SMDTrellisDataCUDA *pTrellis)
{
	kSDMTrellisTestFindNoCandidates<<<1, 64>>>(pTrellis);
}

//-------------------------------------------------------------------------------------------
// SDMTrellisCUDA API
//-------------------------------------------------------------------------------------------

void sdmTrellisFreeStatesCUDA(SMDTrellisDataCUDA *states)
{
    if(states != nullptr)
    {
        cudaFree(states);
    }
}

//-------------------------------------------------------------------------------------------

SMDTrellisDataCUDA *sdmTrellisAllocateCUDA()
{
    SMDTrellisDataCUDA *states = nullptr;

    if(cudaMalloc(&states, sizeof(SMDTrellisDataCUDA)) == cudaSuccess)
    {
        cudaMemset(states, 0, sizeof(SMDTrellisDataCUDA));
    }
    return states;
}

//-------------------------------------------------------------------------------------------

bool sdmTrellisCopyDataCUDA(SMDTrellisDataCUDA *pSrc, SMDTrellisDataCUDA *pDest)
{
    cudaMemcpyKind type = memoryCopyKind(pSrc, pDest);
    return (cudaMemcpy(pDest, pSrc, sizeof(SMDTrellisDataCUDA), type) == cudaSuccess) ? true : false;
}

//-------------------------------------------------------------------------------------------

void initSDMTrellisFilterG_CUDA(const double *g, SMDTrellisDataCUDA *pData)
{
    for(int cIdx = 0; cIdx < 8; cIdx++)
    {
        for(int rIdx = 0; rIdx < 8; rIdx++)
        {
            pData->filterG[cIdx][rIdx] = (rIdx == cIdx || (rIdx - 1) == cIdx) ? 1.0f : 0.0f;
        }
    }
    pData->filterG[1][0] = static_cast<float>(0.0 - g[0]);
    pData->filterG[3][2] = static_cast<float>(0.0 - g[2]);
    pData->filterG[5][4] = static_cast<float>(0.0 - g[4]);
    pData->filterG[7][6] = static_cast<float>(0.0 - g[6]);
}

//-------------------------------------------------------------------------------------------

void initSDMTrellisFilterA_CUDA(const double *a, SMDTrellisDataCUDA *pData)
{
    for(int cIdx = 0; cIdx < 8; cIdx++)
    {
        pData->filterA[cIdx][0] = static_cast<float>(a[cIdx]);
        for(int rIdx = 1; rIdx < 8; rIdx++)
        {
            pData->filterA[cIdx][rIdx] = 0.0f;
        }
    }
}

//-------------------------------------------------------------------------------------------

bool initSDMTrellisFilterCUDA(int dsdRate, bool isClans, SMDTrellisDataCUDA *pData)
{
    if(pData == nullptr)
        return false;
    // Ensure the structure is CPU host memory
    if(isPointerForCUDADevice(pData))
        return false;

    const SDMTrellisFilterBase *filter = getTrellisBaseFilter(dsdRate, isClans);
    if(filter == nullptr)
        return false;
    
    initSDMTrellisFilterA_CUDA(filter->a, pData);
    initSDMTrellisFilterG_CUDA(filter->g, pData);

    for(int i = 0; i < 32; i++)
    {
        filter->path[i] = 0x55555555;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool sdmCUDATrellisRunSingle(SMDTrellisDataCUDA *pTrellis, const float *inSamples, uint8_t *out, int noSamples, bool isLSB, bool isLatencyOutput)
{
    float *inSamplesGPU;
    bool res = false;
    if(cudaMalloc(&inSamplesGPU, noSamples * sizeof(float)) == cudaSuccess)
    {
        if(cudaMemcpy(inSamplesGPU, inSamples, noSamples * sizeof(float), cudaMemcpyHostToDevice) == cudaSuccess)
        {
            uint8_t *outGPU;
            if(cudaMalloc(&outGPU, noSamples / 8) == cudaSuccess)
            {
                kSDMTrellisRun<<<1, 64>>>(pTrellis, inSamplesGPU, outGPU, noSamples, isLSB, isLatencyOutput);
                if(cudaMemcpy(out, outGPU, noSamples / 8, cudaMemcpyDeviceToHost) == cudaSuccess)
                {
                    res = true;
                }
                cudaFree(outGPU);
            }
        }
        cudaFree(inSamplesGPU);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omege
//-------------------------------------------------------------------------------------------
