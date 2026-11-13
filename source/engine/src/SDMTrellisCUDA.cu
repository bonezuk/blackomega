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
#ifdef __SDMTRELLIS_DEBUG_PRINT_OUT
//-------------------------------------------------------------------------------------------

__device__ void kSDMTrellisPrintDebugCurrent(int stageIdx, const uint32_t *path, const float *cCost, const float *cState, int N, float xSample)
{
	if(threadIdx.x == 0)
	{
		printf("=======================================,\n");
		printf("GPU Current, Stage=%d,Candidates=%d, Sample=%.8f,\n", stageIdx, N, xSample);
		printf("=======================================\n");
		printf("index, path, cost, d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],\n");
		for(int i = 0; i < N; i++)
		{
			int idx = i << 3;
			printf("%d, %08x, %.8f, %.8f, %.8f, %.8f, %.8f, %.8f, %.8f, %.8f, %.8f,\n", i, path[i], cCost[i],
				cState[idx + 0], cState[idx + 1], cState[idx + 2], cState[idx + 3],
				cState[idx + 4], cState[idx + 5], cState[idx + 6], cState[idx + 7]);
		}
	}
	__syncthreads();
}

//-------------------------------------------------------------------------------------------

__device__ void kSDMTrellisPrintDebugNext(int stageIdx, const uint32_t *path, const float *nCost, const float *nState, const float *dMatrix, int N, float xSample)
{
	if(threadIdx.x == 0)
	{
		printf("=======================================,\n");
		printf("GPU Next, Stage=%d,Candidates=%d, Sample=%.8f,\n", stageIdx, N, xSample);
		printf("=======================================,\n");
		printf("index, path, cost, d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],\n");
		for(int i = 0; i < N; i++)
		{
			int idx = (i & 0xfffffffe) << 2;
			printf("%d, %08x, %.8f, %.8f, %.8f, %.8f, %.8f, %.8f, %.8f, %.8f, %.8f,\n", i, path[i], nCost[i],
				nState[i], dMatrix[idx + 1], dMatrix[idx + 2], dMatrix[idx + 3],
				dMatrix[idx + 4], dMatrix[idx + 5], dMatrix[idx + 6], dMatrix[idx + 7]);
		}
	}
	__syncthreads();
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

__device__ void kSDMTrellisInitPaths(uint32_t* __restrict__ path)
{
    if(threadIdx.x < 32)
    {
        path[threadIdx.x] = 0x55555555;
    }
}

__device__ void kSDMTrellisLoadPaths(SDMTrellisDataCUDA *pTrellis, uint32_t* __restrict__ path)
{
    if(threadIdx.x < 32)
    {
        path[threadIdx.x] = pTrellis->path[threadIdx.x];
    }
}

__device__ void kSDMTrellisSavePaths(SDMTrellisDataCUDA *pTrellis, uint32_t* __restrict__ path)
{
    if(threadIdx.x < 32)
    {
        pTrellis->path[threadIdx.x] = path[threadIdx.x];
    }
}

//-------------------------------------------------------------------------------------------

__device__ void kSDMTrellisInitCosts(float* __restrict__ cost)
{
    if(threadIdx.x < 32)
    {
        cost[threadIdx.x] = 0.0f;
    }
}

__device__ void kSDMTrellisSaveCosts(SDMTrellisDataCUDA *pTrellis, float* __restrict__ cost)
{
    if(threadIdx.x < 32)
    {
        pTrellis->cost[threadIdx.x] = cost[threadIdx.x];
    }
}

__device__ void kSDMTrellisLoadCosts(SDMTrellisDataCUDA *pTrellis, float* __restrict__ cost)
{
    if(threadIdx.x < 32)
    {
        cost[threadIdx.x] = pTrellis->cost[threadIdx.x];
    }
}

//-------------------------------------------------------------------------------------------

__device__ void kSMDTrellisInitStates(float* __restrict__ states, double firstIntegrator)
{
    int idxS = threadIdx.x << 2;
    states[idxS] = (!(threadIdx.x & 0x1)) ? static_cast<float>(firstIntegrator) : 0.0f;
    #pragma unroll
    for(int i = 1; i < 4; i++)
    {
        states[idxS + i] = 0.0f;
    }   
}

//-------------------------------------------------------------------------------------------

__device__ void kSMDTrellisSaveStates(SDMTrellisDataCUDA *pTrellis, float* __restrict__ states)
{
    float *pMatStateCurr = &(pTrellis->states[0][0][0]);
    int idxS = threadIdx.x << 2;
    #pragma unroll
    for(int i = 0; i < 4; i++)
    {
        pMatStateCurr[idxS + i] = states[idxS + i];
    }   
}

//-------------------------------------------------------------------------------------------

__device__ void kSMDTrellisLoadStates(SDMTrellisDataCUDA *pTrellis, float* __restrict__ states)
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
    const int max = noRows << 3;
    for(int idx = 0; idx < max; idx += 32)
    {
        int i = idx + (threadIdx.x & 0x1f);
        kSplitTF32(inMatrix[i], highM[i], lowM[i]);
    }
}

//-------------------------------------------------------------------------------------------

__device__ __forceinline__ void kSDMTrellisMatrixCalcFP32_16x8x8(const float* __restrict__ A, 
                                                 const uint32_t* __restrict__ hiB, const uint32_t* __restrict__ loB,
                                                 float* __restrict__ D, 
                                                 const int* __restrict__ idxA, const int* __restrict__ idxB, const int* __restrict__ idxC,
                                                 const float xSample)
{
    const int warpIdx = threadIdx.x >> 5;

    __shared__ __align__(16) uint32_t loAMem[2][16 * 8];
    __shared__ __align__(16) uint32_t hiAMem[2][16 * 8];
    uint32_t *loA = &loAMem[warpIdx][0];
    uint32_t *hiA = &hiAMem[warpIdx][0];

    float acc[4];
    float cor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    for(int i = 0; i < 4; i++)
    {
        acc[i] = (!(idxC[i] & 0x7)) ? xSample : 0.0f;
    }

    kSDMTrellisSplitTF32Matrix8x8(A, hiA, loA, 16);
	__syncwarp();

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
    const int tIdx = threadIdx.x & 0x1f;

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
        idxA[i] = tvA(tIdx, i);
        idxC[i] = tvC(tIdx, i);
    }
    for(int i = 0; i < 2; i++)
    {
        idxB[i] = tvB(tIdx, i);
    }
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
    const bool isLower = (!(threadIdx.x & 0x1));
    const bool isNWarp = (threadIdx.x >> 5);

    const float* __restrict__ pCMatState = (!isNWarp) ? cState : &cState[16 * 8];
    float* __restrict__ pDMatrix = (!isNWarp) ? dMatrix : &dMatrix[16 * 8];
    float* __restrict__ pVMatrix = (!isNWarp) ? vMatrix : &vMatrix[16 * 8];

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

    int dIdx = (threadIdx.x & 0xfffffffe) << 2;
    nStateA[threadIdx.x] = dMatrix[dIdx] + ((isLower) ? 1.0f : -1.0f);

    // Cost is the squared quantisation error (v - y)^2, matching SDMTrellisSIMD.cxx.
    float v = vMatrix[dIdx] + ((isLower) ? 1.0f : -1.0f);
    (void)a0;
    nCost[threadIdx.x] = cCost[threadIdx.x >> 1] + (v * v);

    __syncthreads();
}
//-------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------

__device__ void kSDMTrellisSplitTF32Matrix8x8(const float *__restrict__ inMatrix,
    uint32_t *__restrict__ highM,
    uint32_t *__restrict__ lowM,
    int noRows)
{}

//-------------------------------------------------------------------------------------------

__device__ void kSDMTrellisMMALayout(int *__restrict__ idxA, int *__restrict__ idxB, int *__restrict__ idxC)
{}

//-------------------------------------------------------------------------------------------

__device__ __forceinline__ void kSDMTrellisCalcStateAndCostFP32(const float *__restrict__ cState,
    const uint32_t *__restrict__ matLoAFilter, const uint32_t *__restrict__ matHiAFilter,
    const uint32_t *__restrict__ matLoGFilter, const uint32_t *__restrict__ matHiGFilter,
    const int *__restrict__ idxA, const int *__restrict__ idxB, const int *__restrict__ idxC,
    float *__restrict__ dMatrix, float *__restrict__ vMatrix, float *__restrict__ nStateA,
    const float *__restrict__  cCost, float *__restrict__ nCost,
    const float a0, float xSample)
{}

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

__device__ __forceinline__ void kSDMTrellisRankSortCosts(const float c, int N, uint32_t* __restrict__ order)
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

    order[rank] = (i < N && ki != 0xffffffff) ? i : 0xffffffff;
    __syncthreads();
}

//-------------------------------------------------------------------------------------------

__device__ __forceinline__ void kSDMTrellisRankSortPath(const uint32_t nState, int N, uint32_t* __restrict__ order)
{
    const int i = threadIdx.x;

    __shared__ __align__(16) uint32_t keys[64];
    const uint32_t ki = (i < N) ? nState : 0xffffffff;
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
    const int i = threadIdx.x;

    // create ordered index of states
    __shared__ __align__(16) uint32_t order[64];
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
    const int i = threadIdx.x;

    // step path to next from current
    nPath[i] = (cPath[i >> 1] << 1) | ((i & 0x1) ? 1 : 0);
        
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

__global__ void kSDMTrellisRunOld(SDMTrellisDataCUDA *pTrellis, const float *inSamples, uint8_t *outData, int noSamples, bool isLSB, bool isLatencyOutput)
{
	int noBlockSamples = noSamples / gridDim.x;
	int startIdx = noBlockSamples * blockIdx.x;
	uint8_t *output = &outData[startIdx / 8];
	int endIdx = startIdx + noBlockSamples + 32;
	if(blockIdx.x > 0)
	{
		startIdx -= 32;
	}
	if(endIdx > noSamples)
	{
		endIdx = noSamples;
	}

    // 64 * 2 * 4 = 512 : S 512
    __shared__ __align__(16) uint32_t matLoAFilter[64], matHiAFilter[64]; 
    // 64 * 2 * 4 = 512 : S 1024
    __shared__ __align__(16) uint32_t matLoGFilter[64], matHiGFilter[64];
    // 32 * 4 = 128 : S 1152
    __shared__ __align__(16) uint32_t cPath[32];
    // 64 * 4 = 256 : S 1408
    __shared__ __align__(16) uint32_t nPath[64];
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

    float *pAFilter = &(pTrellis->filterA[0][0]);
    float *pGFilter = &(pTrellis->filterG[0][0]);
    if(threadIdx.x < 32)
    {
        kSDMTrellisSplitTF32Matrix8x8(pAFilter, matHiAFilter, matLoAFilter, 8);
        kSDMTrellisSplitTF32Matrix8x8(pGFilter, matHiGFilter, matLoGFilter, 8);
    }
    float a0 = pTrellis->filterA[0][0];

    int idxA[4], idxB[2], idxC[4];
    kSDMTrellisMMALayout(idxA, idxB, idxC);

    kSDMTrellisLoadPaths(pTrellis, cPath);
    kSMDTrellisLoadStates(pTrellis, cState);
    kSDMTrellisLoadCosts(pTrellis, cCost);

    uint32_t trellisMask = (1 << pTrellis->order) - 1;
    int noCandidates = pTrellis->noCandidates;
    __syncthreads();

	const uint8_t maskMSB[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
	const uint8_t maskLSB[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
    const uint8_t *mask = (isLSB) ? maskLSB : maskMSB;
    int noOutSamples = 0;
    int minCostIndex = 0;
    uint8_t dsd = 0;

    const int i = threadIdx.x;

#ifdef __SDMTRELLIS_DEBUG_PRINT_OUT
    for(int sampleIdx = 0; sampleIdx < __SDMTRELLIS_DEBUG_PRINT_NOSAMPLES; sampleIdx++)
#else
    for(int sampleIdx = startIdx; sampleIdx < endIdx; sampleIdx++)
#endif
    {
        float sample = inSamples[sampleIdx] * c_SDMTrellisGain;

#ifdef __SDMTRELLIS_DEBUG_PRINT_OUT
		kSDMTrellisPrintDebugCurrent(sampleIdx, cPath, cCost, cState, noCandidates, sample);
#endif

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
        minCostIndex = kSDMTrellisMinIndex(nCost, noCandidates);
        float minCost = nCost[minCostIndex];

        // Find the output bit for this step at latency=32 based upon the minimum cost
        int iA = minCostIndex >> 1;
        int outputBit = (cPath[iA] & 0x80000000) ? 1 : 0;

        // Perform Viberti and get new candidates.
        noCandidates = kSDMTrellisVibertiStep(nPath, noCandidates, cPath, nCost, indices, trellisMask, outputBit);

#ifdef __SDMTRELLIS_DEBUG_PRINT_OUT
		kSDMTrellisPrintDebugNext(sampleIdx, nPath, nCost, nStateA, dMatrix, noCandidates, sample);
#endif

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
                output[(noOutSamples >> 3) - 1] = dsd;
                dsd = 0;
            }
        }
        __syncthreads();
    }
    
    if(i == 32 && blockIdx.x == (gridDim.x - 1))
    {
    	uint32_t lPath = cPath[0];
    	for(int j = 31; j >= 0; j--)
    	{
    		uint32_t lMask = 1 << j;
    		if(lPath & lMask)
    		{
    			dsd |= mask[noOutSamples & 0x7];
    		}
    		noOutSamples++;
            if(!(noOutSamples & 0x7))
            {
                output[(noOutSamples >> 3) - 1] = dsd;
                dsd = 0;
            }
    	}
    }
}

//-------------------------------------------------------------------------------------------

__device__ void kSDMTrellisRunOnBlock(SDMTrellisDataCUDA *pTrellis, const float *in, uint8_t *out, int start, int to,
                                    uint32_t* __restrict__ cPath, float* __restrict__ cState, float* __restrict__ cCost,
                                    int noCandidates)
{
    // 64 * 2 * 4 = 512 : S 512
    __shared__ __align__(16) uint32_t matLoAFilter[64], matHiAFilter[64]; 
    // 64 * 2 * 4 = 512 : S 1024
    __shared__ __align__(16) uint32_t matLoGFilter[64], matHiGFilter[64];
    // 64 * 4 = 256 : S 1408
    __shared__ __align__(16) uint32_t nPath[64];
    // 32 * 8 * 4 = 1024 : S 2432
    __shared__ __align__(16) float dMatrix[32 * 8];
    // 32 * 8 * 4 = 1024 : S 3456
    __shared__ __align__(16) float vMatrix[32 * 8];
    // 32 * 4 = 256 : S 4736
    __shared__ __align__(16) float nStateA[64];
    // 64 * 4 = 256 : S 5120
    __shared__ __align__(16) float nCost[64];
    // 64 * 4 = 256 : S 5376
    __shared__ __align__(16) uint32_t indices[64];

    float *pAFilter = &(pTrellis->filterA[0][0]);
    float *pGFilter = &(pTrellis->filterG[0][0]);
    if(threadIdx.x < 32)
    {
        kSDMTrellisSplitTF32Matrix8x8(pAFilter, matHiAFilter, matLoAFilter, 8);
        kSDMTrellisSplitTF32Matrix8x8(pGFilter, matHiGFilter, matLoGFilter, 8);
    }
    float a0 = pTrellis->filterA[0][0];

    int idxA[4], idxB[2], idxC[4];
    kSDMTrellisMMALayout(idxA, idxB, idxC);

    uint32_t trellisMask = (1 << pTrellis->order) - 1;
    __syncthreads();

    int noOutSamples = 0;
    int minCostIndex = 0;

    const int i = threadIdx.x;

    for(int sampleIdx = start; sampleIdx < to; sampleIdx++)
    {
        float sample = in[sampleIdx] * c_SDMTrellisGain;

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
        minCostIndex = kSDMTrellisMinIndex(nCost, noCandidates);
        float minCost = nCost[minCostIndex];

        // Find the output bit for this step at latency=32 based upon the minimum cost
        int iA = minCostIndex >> 1;
        int outputBit = (cPath[iA] & 0x80000000) ? 1 : 0;

        // Perform Viberti and get new candidates.
        noCandidates = kSDMTrellisVibertiStep(nPath, noCandidates, cPath, nCost, indices, trellisMask, outputBit);

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

        if(i == 32)
        {
            out[noOutSamples] = (outputBit) ? 0x01 : 0x00;
            noOutSamples++;
        }
        __syncthreads();
    }
}

//-------------------------------------------------------------------------------------------

__device__ double kSDMTrellisIntegratorInvariant(const SDMTrellisDataCUDA *pTrellis)
{
    const double g0 = 0.0 - static_cast<double>(pTrellis->filterG[1][0]);
    const double g2 = 0.0 - static_cast<double>(pTrellis->filterG[3][2]);
    const double g4 = 0.0 - static_cast<double>(pTrellis->filterG[5][4]);
    const double sA = static_cast<double>(pTrellis->states[0][0][0]);
    const double sB = static_cast<double>(pTrellis->states[0][0][2]);
    const double sC = static_cast<double>(pTrellis->states[0][0][4]);
    const double sD = static_cast<double>(pTrellis->states[0][0][8]);
    double val = sA + (g0 * sB) + (g0 * g2 * sC) + (g0 * g2 * g4 * sD);
    return val;
}

//-------------------------------------------------------------------------------------------

__device__ __forceinline__ float kSDMTrellisWarpSum(float v)
{
    #pragma unroll
    for(int offset = 16; offset > 0; offset >>= 1)
    {
        v += __shfl_xor_sync(0xffffffff, v, offset);
    }
    return v;
}

//-------------------------------------------------------------------------------------------

__device__ double kSDMTrellisFirstIntegrator(const SDMTrellisBlockDataCUDA *pTrellis, const float *in)
{
    const int i = threadIdx.x & 0x1f;
    double w = 0.0f;

    if(blockIdx.x > 0 && blockIdx.x < pTrellis->noBlocks)
    {
        double prefix = 0.0;
        int start = pTrellis->block[blockIdx.x].start;
        for(int pos = 0; pos < start; pos += 64)
        {
            float v = 0.0f;

            if((pos + i) < start)
            {
                v += in[pos + i] * c_SDMTrellisGain;
                if((pos + i + 32) < start)
                {
                    v += in[pos + i + 32] * c_SDMTrellisGain;
                }
            }
            v = kSDMTrellisWarpSum(v);
            prefix += v;
        }

        const double J0 = kSDMTrellisIntegratorInvariant(&pTrellis->trellis);
        w = fmod(J0 + prefix - static_cast<double>(start & 1), 2.0);
        if(w >= 1.0)
        {
            w -= 2.0;
        }
        else if(w < -1.0)
        {
            w += 2.0;
        }
    }
    return w;
}

//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisRun(SDMTrellisBlockDataCUDA *pBlock,  const float *in, SDMTrellisBlockOutputCUDA *pOutput)
{
    SDMTrellisDataCUDA *pTrellis = &pBlock->trellis;

    // 32 * 4 = 128 : S 1152
    __shared__ __align__(16) uint32_t cPath[32];
    // 32 * 8 * 4 = 1024 : S 4480
    __shared__ __align__(16) float cState[32 * 8];
    // 32 * 4 = 128 : S 4864
    __shared__ __align__(16) float cCost[32];

    int noCandidates;
    if(blockIdx.x == 0)
    {
        kSDMTrellisLoadPaths(pTrellis, cPath);
        kSMDTrellisLoadStates(pTrellis, cState);
        kSDMTrellisLoadCosts(pTrellis, cCost);
        noCandidates = pTrellis->noCandidates;
    }
    else
    {
        double firstIntegrator = kSDMTrellisFirstIntegrator(pBlock, in);
        kSDMTrellisInitPaths(cPath);
        kSMDTrellisInitStates(cState, firstIntegrator);
        kSDMTrellisInitCosts(cCost);
        noCandidates = 1;
    }
    __syncthreads();

    int start = pBlock->block[blockIdx.x].start;
    int to = pBlock->block[blockIdx.x].to;
    uint8_t *out = pOutput->data[blockIdx.x];
    assert(pOutput->allocated >= (to - start));

    kSDMTrellisRunOnBlock(&pBlock->trellis, in, out, start, to,
        cPath, cState, cCost, noCandidates);
}

//-------------------------------------------------------------------------------------------
// Supporting kernels for unit testing
//-------------------------------------------------------------------------------------------
#if defined(__OMEGA_CUDA_SM80)
//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestCalcStateAndCostFP32(SDMTrellisDataCUDA *pTrellis, float xSample)
{
    __shared__ __align__(16) uint32_t matLoAFilter[64], matHiAFilter[64];
    __shared__ __align__(16) uint32_t matLoGFilter[64], matHiGFilter[64];
    float *pAFilter = &(pTrellis->filterA[0][0]);
    float *pGFilter = &(pTrellis->filterG[0][0]);
    if(threadIdx.x < 32)
    {
        kSDMTrellisSplitTF32Matrix8x8(pAFilter, matHiAFilter, matLoAFilter, 8);
        kSDMTrellisSplitTF32Matrix8x8(pGFilter, matHiGFilter, matLoGFilter, 8);
	}
    float a0 = pTrellis->filterA[0][0];
	__syncthreads();

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
        a0, xSample);
    
    int dIdx = (threadIdx.x & 0xfffffffe) << 2;
    pTrellis->states[1][threadIdx.x][0] = nStateA[threadIdx.x];
    for(int i = 1; i < 8; i++)
    {
        pTrellis->states[1][threadIdx.x][i] = dMatrix[dIdx + i];
    }

    pTrellis->cost[threadIdx.x] = nCost[threadIdx.x];
}

//-------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestCalcStateAndCostFP32(SDMTrellisDataCUDA *pTrellis, float xSample)
{}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestRankSortPath(SDMTrellisDataCUDA *pTrellis)
{
    const int i = threadIdx.x;
    uint32_t state = pTrellis->path[i];
    int N = pTrellis->noCandidates;
    __shared__ __align__(16) uint32_t order[64];
    kSDMTrellisRankSortPath(state, N, order);
    if(i < N)
    {
        pTrellis->path[i] = pTrellis->path[order[i]];
    }
}

//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestFindNoCandidates(SDMTrellisDataCUDA *pTrellis)
{
	const int i = threadIdx.x;
	__shared__ __align__(16) uint32_t indices[64];
	indices[i] = pTrellis->path[i];
	pTrellis->path[i] = kSDMTrellisFindNoCandidates(indices, pTrellis->noCandidates);
}

//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestPruneStatePairs(SDMTrellisDataCUDA *pTrellis)
{
    __shared__ float cost[64];
    __shared__ uint32_t states[64];
    __shared__ uint32_t indices[64];
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

__global__ void kTestSDMTrellisRankSortCosts(SDMTrellisDataCUDA *pTrellis)
{
    __shared__ float cost[64];
    __shared__ uint32_t order[64];
    int N = pTrellis->noCandidates;

    if(threadIdx.x < N)
    {
        cost[threadIdx.x] = pTrellis->cost[threadIdx.x];
        order[threadIdx.x] = threadIdx.x;
    }
    __syncthreads();

    kSDMTrellisRankSortCosts(cost[threadIdx.x], N, order);

    pTrellis->path[threadIdx.x] = order[threadIdx.x];
}

//-------------------------------------------------------------------------------------------

__global__ void kTestSDMTrellisRankSortCostsWithBlanks(SDMTrellisDataCUDA *pTrellis)
{
    __shared__ float cost[64];
    __shared__ uint32_t order[64];
    int N = pTrellis->noCandidates;

    if(threadIdx.x < N)
    {
        cost[threadIdx.x] = pTrellis->cost[threadIdx.x];
        order[threadIdx.x] = pTrellis->path[threadIdx.x];
    }
    __syncthreads();

    kSDMTrellisRankSortCosts(cost[threadIdx.x], N, order);

    pTrellis->path[threadIdx.x] = order[threadIdx.x];
}

//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestMinIndex(SDMTrellisDataCUDA *pTrellis)
{
    int N = pTrellis->noCandidates;
    __shared__ __align__(16) float cost[64];
    cost[threadIdx.x] = pTrellis->cost[threadIdx.x];
    __syncthreads();
    int minIdx = kSDMTrellisMinIndex(cost, N);
    pTrellis->path[threadIdx.x] = static_cast<uint32_t>(minIdx);
}

//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestVibertiStep(SDMTrellisDataCUDA *pTrellis)
{
    const int i = threadIdx.x;
    __shared__ uint32_t cPath[32], nPath[64];
    __shared__ float nCost[64];
    __shared__ uint32_t indices[64];
    if(i < 32)
    {
        cPath[i] = pTrellis->path[i];
    }
    nCost[i] = pTrellis->cost[i];
    int N = pTrellis->noCandidates << 1;
    int trellisMask = (1 << pTrellis->order) - 1;
    __syncthreads();

    N = kSDMTrellisVibertiStep(nPath, N, cPath, nCost, indices, trellisMask, 0);
    if(i < N)
    {
        int sIdx = indices[i];
        pTrellis->cost[i] = nCost[sIdx];
        pTrellis->path[i] = nPath[sIdx] & trellisMask;
    }
    if(!i)
    {
        pTrellis->noCandidates = N;
    }
}

//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestIntegratorInvariant(SDMTrellisDataCUDA *pTrellis)
{
    pTrellis->cost[threadIdx.x] = kSDMTrellisIntegratorInvariant(pTrellis);
}

//-------------------------------------------------------------------------------------------

__global__ void kSDMTrellisTestFirstIntegrator(const SDMTrellisBlockDataCUDA *pTrellis, const float *in, double *integrator)
{
    double v = kSDMTrellisFirstIntegrator(pTrellis, in);
    if(threadIdx.x == 0)
    {
        integrator[blockIdx.x] = v;
    }
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisCalcStateAndCostCUDAFP32(SDMTrellisDataCUDA *pTrellis, float xSample)
{
    kSDMTrellisTestCalcStateAndCostFP32<<<1, 64>>>(pTrellis, xSample);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisRankSortCosts(SDMTrellisDataCUDA *pTrellis)
{
    kTestSDMTrellisRankSortCosts<<<1, 64>>>(pTrellis);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisMinIndex(SDMTrellisDataCUDA *pTrellis)
{
    kSDMTrellisTestMinIndex<<<1, 64>>>(pTrellis);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisRankSortPath(SDMTrellisDataCUDA *pTrellis)
{
    kSDMTrellisTestRankSortPath<<<1, 64>>>(pTrellis);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisRankSortCostsWithBlanks(SDMTrellisDataCUDA *pTrellis)
{
    kTestSDMTrellisRankSortCostsWithBlanks<<<1, 64>>>(pTrellis);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisVibertiStep(SDMTrellisDataCUDA *pTrellis)
{
    kSDMTrellisTestVibertiStep<<<1, 64>>>(pTrellis);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisPruneStatePairs(SDMTrellisDataCUDA *pTrellis)
{
	kSDMTrellisTestPruneStatePairs<<<1, 64>>>(pTrellis);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisFindNoCandidates(SDMTrellisDataCUDA *pTrellis)
{
	kSDMTrellisTestFindNoCandidates<<<1, 64>>>(pTrellis);
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisIntegratorInvariant(SDMTrellisDataCUDA *pTrellis)
{
    kSDMTrellisTestIntegratorInvariant<<<1, 64>>>(pTrellis);
}

//-------------------------------------------------------------------------------------------

double *testSDMTrellisFirstIntegrator(const SDMTrellisBlockDataCUDA *pTrellis, const float *pcm, int noSamples, int noBlocks)
{
    double *integratorCPU = new double [noBlocks];
    double *integratorGPU = NULL;
    assert(cudaMalloc(&integratorGPU, sizeof(double) * noBlocks) == cudaSuccess);
    assert(integratorGPU != NULL);
    float *pcmGPU = NULL;
    assert(cudaMalloc(&pcmGPU, sizeof(float) * noSamples) == cudaSuccess);
    assert(cudaMemcpy(pcmGPU, pcm, sizeof(float) * noSamples, cudaMemcpyHostToDevice) == cudaSuccess);

    kSDMTrellisTestFirstIntegrator<<<noBlocks, 64>>>(pTrellis, pcmGPU, integratorGPU);
    
    assert(cudaMemcpy(integratorCPU, integratorGPU, sizeof(double) * noBlocks, cudaMemcpyDeviceToHost) == cudaSuccess);
    cudaFree(integratorGPU);
    cudaFree(pcmGPU);
    return integratorCPU;
}

//-------------------------------------------------------------------------------------------

void testSDMTrellisRun(SDMTrellisBlockDataCUDA *pBlock,  const float *in, SDMTrellisBlockOutputCUDA *pOutput, int noBlocks)
{
    kSDMTrellisRun<<<noBlocks, 64>>>(pBlock, in, pOutput);
}

//-------------------------------------------------------------------------------------------
// SDMTrellisCUDA API
//-------------------------------------------------------------------------------------------

void sdmTrellisFreeStatesCUDA(SDMTrellisDataCUDA *states)
{
    if(states != nullptr)
    {
        cudaFree(states);
    }
}

//-------------------------------------------------------------------------------------------

SDMTrellisDataCUDA *sdmTrellisAllocateCUDA()
{
    SDMTrellisDataCUDA *states = nullptr;

    if(cudaMalloc(&states, sizeof(SDMTrellisDataCUDA)) == cudaSuccess)
    {
        cudaMemset(states, 0, sizeof(SDMTrellisDataCUDA));
    }
    return states;
}

//-------------------------------------------------------------------------------------------

bool sdmTrellisCopyDataCUDA(SDMTrellisDataCUDA *pSrc, SDMTrellisDataCUDA *pDest)
{
    cudaMemcpyKind type = memoryCopyKind(pSrc, pDest);
    return (cudaMemcpy(pDest, pSrc, sizeof(SDMTrellisDataCUDA), type) == cudaSuccess) ? true : false;
}

//-------------------------------------------------------------------------------------------

void initSDMTrellisFilterG_CUDA(const double *g, SDMTrellisDataCUDA *pData)
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

void initSDMTrellisFilterA_CUDA(const double *a, SDMTrellisDataCUDA *pData)
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

bool initSDMTrellisCUDA(int dsdRate, int order, bool isClans, SDMTrellisDataCUDA *pData)
{
    if(pData == nullptr)
        return false;
    // Ensure the structure is CPU host memory
    if(isPointerForCUDADevice(pData))
    {
        SDMTrellisDataCUDA trellisCPU;
        bool res = initSDMTrellisCUDA(dsdRate, order, isClans, &trellisCPU);
        if(res)
        {
            res = sdmTrellisCopyDataCUDA(&trellisCPU, pData);
        }
        return res;
    }

	if(!(order > 0 && order < 31))
	{
		return false;
	}

    memset(pData, 0, sizeof(SDMTrellisDataCUDA));

    const SDMTrellisFilterBase *filter = getTrellisBaseFilter(dsdRate, isClans);
    if(filter == nullptr)
        return false;
    
    initSDMTrellisFilterA_CUDA(filter->a, pData);
    initSDMTrellisFilterG_CUDA(filter->g, pData);

    for(int i = 0; i < 32; i++)
    {
        pData->path[i] = 0x55555555;
    }
    pData->order = order;
	pData->noCandidates = 1;
	pData->rate = dsdRate;
    return true;
}

//-------------------------------------------------------------------------------------------

bool initSDMTrellisBlockCUDA(int dsdRate, int order, bool isClans, SDMTrellisBlockDataCUDA *pData)
{
    if(pData == nullptr)
        return false;

    if(isPointerForCUDADevice(pData))
    {
        SDMTrellisBlockDataCUDA blockCPU;
        bool res = initSDMTrellisBlockCUDA(dsdRate, order, isClans, &blockCPU);
        if(res)
        {
            copySDMTrellisCUDABlockData(&blockCPU, pData);
        }
        return res;
    }
    
    memset(pData, 0, sizeof(SDMTrellisBlockDataCUDA));
    if(!initSDMTrellisCUDA(dsdRate, order, isClans, &pData->trellis) || !initSDMTrellisCUDA(dsdRate, order, isClans, &pData->lastTrellis))
        return false;
    return true;
}

//-------------------------------------------------------------------------------------------

bool sdmCUDATrellisRunSingle(SDMTrellisDataCUDA *pTrellis, const float *inSamples, uint8_t *out, int noSamples, bool isLSB, bool isLatencyOutput)
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
                kSDMTrellisRunOld<<<1, 64>>>(pTrellis, inSamplesGPU, outGPU, noSamples, isLSB, isLatencyOutput);
                
				if(cudaDeviceSynchronize() == cudaSuccess)
				{
					if(cudaMemcpy(out, outGPU, noSamples / 8, cudaMemcpyDeviceToHost) == cudaSuccess)
					{
						res = true;
					}
				}
                cudaFree(outGPU);
            }
        }
        cudaFree(inSamplesGPU);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool sdmCUDATrellisRun(SDMTrellisDataCUDA *pTrellis, const float *inSamples, uint8_t *out, int noSamples, bool isLSB, int noTrellisStreams)
{
	int noOutputSamples = noSamples / 8;
	uint8_t *outGPU;
	bool res = false;
	
	if(isPointerForCUDADevice(inSamples) > 0)
	{
		if(isPointerForCUDADevice(out) < 1)
		{
			if(cudaMalloc(&outGPU, noOutputSamples * sizeof(uint8_t)) != cudaSuccess)
			{
				return false;
			}
		}
		else
		{
			outGPU = out;
		}
		
		kSDMTrellisRunOld<<<noTrellisStreams, 64>>>(pTrellis, inSamples, outGPU, noSamples, isLSB, false);
		
		if(isPointerForCUDADevice(out) < 1)
		{
			if(cudaMemcpy(out, outGPU, noOutputSamples * sizeof(uint8_t), cudaMemcpyDeviceToHost) == cudaSuccess)
			{
				res = true;
			}
			cudaFree(outGPU);
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

int sdmTrellisCUDAQueryBlockDimensions(int noChannels)
{
    int devID, perSM, noBlocks;
    cudaDeviceProp deviceProp;
        
    noBlocks = -1;
    devID = initCUDAOmega();
    if(devID >= 0)
    {
        if(cudaGetDeviceProperties(&deviceProp, devID) == cudaSuccess)
        {
            if(cudaOccupancyMaxActiveBlocksPerMultiprocessor(&perSM, kSDMTrellisRun, 64, 0) == cudaSuccess)
            {
                noBlocks = (deviceProp.multiProcessorCount * perSM) / noChannels;
            }
        }
    }
    return noBlocks;
}

//-------------------------------------------------------------------------------------------

bool sdmTrellisCUDACarryOverLastTrellisToNextBlock(SDMTrellisBlockDataCUDA *pBlockTrellis)
{
    return (cudaMemcpy(&pBlockTrellis->trellis, &pBlockTrellis->lastTrellis, sizeof(SDMTrellisDataCUDA), memoryCopyKind(&pBlockTrellis->lastTrellis, &pBlockTrellis->trellis)) == cudaSuccess) ? true : false;
}

//-------------------------------------------------------------------------------------------

void sdmTrellisCUDACalcBlockSize(SDMTrellisBlockDataCUDA *pBTrellis, int noSamples)
{
    int noBlocks, blockSize, halfOverlay;

    noBlocks = noSamples / pBTrellis->prefBlockSamples;
    if((noSamples % pBTrellis->prefBlockSamples) != 0)
    {
        noBlocks++;
    }
    if(noBlocks > c_SDMTrellisMaxNoBlocks)
    {
        noBlocks = c_SDMTrellisMaxNoBlocks;
    }
    blockSize = noSamples / noBlocks;
    halfOverlay = pBTrellis->overlay >> 1;

    for(int i = 0; i < noBlocks; i++)
    {
        int posS = blockSize * i;
        int posE = blockSize * (i + 1);
        int from = posS - halfOverlay;
        int start = from - pBTrellis->warmup;
        if(from < 0)
            from = 0;
        if(start < 0)
            start = 0;
        int to;
        if(i < (noBlocks - 1))
        {
            to = posE + halfOverlay;
            if(to > noSamples)
                to = noSamples;
        }
        else
        {
            to = noSamples;
        }
        pBTrellis->block[i].start = start;
        pBTrellis->block[i].from = from;
        pBTrellis->block[i].to = to;
    }
    pBTrellis->noBlocks = noBlocks;
}

//-------------------------------------------------------------------------------------------

int sdmTrellisCUDABlockOutputSize(SDMTrellisBlockDataCUDA *pBTrellis, int idx)
{
    return pBTrellis->block[idx].to - pBTrellis->block[idx].from;
}

//-------------------------------------------------------------------------------------------

int sdmTrellisCUDABlockOuputSize(SDMTrellisBlockDataCUDA *pBTrellis)
{
    return pBTrellis->prefBlockSamples + (2 * pBTrellis->overlay) + pBTrellis->warmup;
}

//-------------------------------------------------------------------------------------------

void freeSDMTrellisCUDABlockOutput(SDMTrellisBlockOutputCUDA *pOutput)
{
    if(pOutput != NULL)
    {
        if(isPointerForCUDADevice(pOutput))
        {
            SDMTrellisBlockOutputCUDA block;
            if(cudaMemcpy(&block, pOutput, sizeof(SDMTrellisBlockOutputCUDA), memoryCopyKind(&block, pOutput)) == cudaSuccess)
            {
                for(int i = 0; i < c_SDMTrellisMaxNoBlocks; i++)
                {
                    if(block.data[i] != NULL)
                    {
                        cudaFree(block.data[i]);
                    }
                }
            }
            cudaFree(pOutput);
        }
        else
        {
            for(int i = 0; i < c_SDMTrellisMaxNoBlocks; i++)
            {
                if(pOutput->data[i] != NULL)
                {
                    free(pOutput->data[i]);
                }
            }
            free(pOutput);
        }
    }
}

//-------------------------------------------------------------------------------------------

SDMTrellisBlockOutputCUDA *allocateSDMTrellisCUDABlockOuput(SDMTrellisBlockDataCUDA *pBTrellis, bool isDevice)
{
    SDMTrellisBlockOutputCUDA *pOutput = NULL;
    bool res = false;

    if(isDevice)
    {
        if(cudaMalloc(&pOutput, sizeof(SDMTrellisBlockOutputCUDA)) == cudaSuccess)
        {
            
            SDMTrellisBlockOutputCUDA block;

            cudaMemset(pOutput, 0, sizeof(SDMTrellisBlockOutputCUDA));
            block.allocated = sdmTrellisCUDABlockOuputSize(pBTrellis);
            res = true;
            for(int i = 0; i < c_SDMTrellisMaxNoBlocks && res; i++)
            {
                if(cudaMalloc(&block.data[i], block.allocated) != cudaSuccess)
                {
                    res = false;
                }
            }
            if(res)
            {
                if(cudaMemcpy(pOutput, &block, sizeof(SDMTrellisBlockOutputCUDA), cudaMemcpyHostToDevice) != cudaSuccess)
                {
                    res = false;
                }
            }
        }
    }
    else
    {
        pOutput = reinterpret_cast<SDMTrellisBlockOutputCUDA *>(calloc(1, sizeof(SDMTrellisBlockOutputCUDA)));
        if(pOutput != NULL)
        {
            pOutput->allocated = sdmTrellisCUDABlockOuputSize(pBTrellis);
            res = true;
            for(int i = 0; i < c_SDMTrellisMaxNoBlocks && res; i++)
            {
                pOutput->data[i] = reinterpret_cast<uint8_t *>(calloc(pOutput->allocated, sizeof(uint8_t)));
                if(pOutput->data[i] == NULL)
                {
                    res = false;
                }
            }
        }
    }
    if(!res)
    {
        freeSDMTrellisCUDABlockOutput(pOutput);
        pOutput = NULL;
    }
    return pOutput;
}

//-------------------------------------------------------------------------------------------

bool copySDMTrellisCUDABlockOutput(SDMTrellisBlockOutputCUDA *src, SDMTrellisBlockOutputCUDA *dest, int noBlocks)
{
    SDMTrellisBlockOutputCUDA sBlock, dBlock;
    bool res = false;

    if(cudaMemcpy(&sBlock, src, sizeof(SDMTrellisBlockOutputCUDA), memoryCopyKind(src, &sBlock)) == cudaSuccess)
    {
        if(cudaMemcpy(&dBlock, dest, sizeof(SDMTrellisBlockOutputCUDA), memoryCopyKind(dest, &dBlock)) == cudaSuccess)
        {
            if(sBlock.allocated == dBlock.allocated)
            {
                res = true;
                for(int i = 0; i < noBlocks && res; i++)
                {
                    if(cudaMemcpy(dBlock.data[i], sBlock.data[i], sBlock.allocated, memoryCopyKind(sBlock.data[i], dBlock.data[i])) != cudaSuccess)
                    {
                        res = false;
                    }
                }
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void freeSDMTrellisCUDABlockData(SDMTrellisBlockDataCUDA *pBTrellis)
{
    if(pBTrellis != NULL)
    {
        cudaFree(pBTrellis);
    }
}

//-------------------------------------------------------------------------------------------

SDMTrellisBlockDataCUDA *allocateSDMTrellisCUDABlockData()
{
    SDMTrellisBlockDataCUDA *pBTrellis = NULL;
    if(cudaMalloc(&pBTrellis, sizeof(SDMTrellisBlockDataCUDA)) != cudaSuccess)
    {
        freeSDMTrellisCUDABlockData(pBTrellis);
        pBTrellis = NULL;
    }
    return pBTrellis;
}

//-------------------------------------------------------------------------------------------

void copySDMTrellisCUDABlockData(const SDMTrellisBlockDataCUDA *src, SDMTrellisBlockDataCUDA *dest)
{
    cudaMemcpy(dest, src, sizeof(SDMTrellisBlockDataCUDA), memoryCopyKind(src, dest));
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omege
//-------------------------------------------------------------------------------------------
