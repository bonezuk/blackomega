// argmin64.cu
// Find the index of the minimum value in an array of up to 64 floats.
//
// Build:  nvcc -O3 -o argmin64 argmin64.cu      (needs CUDA 9+ for __shfl_*_sync)

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cuda_runtime.h>
#include <math_constants.h>   // CUDART_INF_F

// ---------------------------------------------------------------------------
// Option 1: single-thread version.
// Use when each thread owns its own small array.
// Returns -1 if n <= 0. Ties resolve to the lowest index. NaNs are ignored.
// ---------------------------------------------------------------------------
__device__ __forceinline__ int argMinSerial(const float* a, int n)
{
    int   best = (n > 0) ? 0 : -1;
    float bv   = CUDART_INF_F;
    for (int i = 0; i < n; ++i) {
        float v = a[i];
        if (v < bv) { bv = v; best = i; }   // strict '<' keeps the first minimum
    }
    return best;
}

// ---------------------------------------------------------------------------
// Option 2: warp-cooperative version (fast).
// Must be called by all 32 threads of a warp together. Every lane gets the
// result. Each lane loads 2 elements (lane, lane+32), then a 5-step shuffle
// reduction finds the minimum. No shared memory, no __syncthreads().
// Returns -1 if n <= 0. Ties resolve to the lowest index. NaNs are ignored.
// ---------------------------------------------------------------------------
__device__ __forceinline__ int warpArgMin64(const float* vals, int n)
{
    if (n <= 0) return -1;                 // n is uniform across the warp
    const unsigned FULL = 0xffffffffu;
    const int lane = threadIdx.x & 31;

    // Load two candidates; out-of-range or NaN entries become +inf.
    const int i0 = lane, i1 = lane + 32;
    float v0 = (i0 < n) ? vals[i0] : CUDART_INF_F;
    float v1 = (i1 < n) ? vals[i1] : CUDART_INF_F;
    if (isnan(v0)) v0 = CUDART_INF_F;
    if (isnan(v1)) v1 = CUDART_INF_F;

    float best = v0;
    int   idx  = i0;
    if (v1 < best) { best = v1; idx = i1; }

    // Butterfly reduction: after 5 steps every lane holds the warp-wide min.
    #pragma unroll
    for (int mask = 16; mask > 0; mask >>= 1) {
        float ov = __shfl_xor_sync(FULL, best, mask);
        int   oi = __shfl_xor_sync(FULL, idx,  mask);
        if (ov < best || (ov == best && oi < idx)) { best = ov; idx = oi; }
    }
    return idx;
}

// ---------------------------------------------------------------------------
// Example kernel: one warp per array. Arrays are stored back to back with a
// stride of 64 floats; each holds n valid values (n <= 64).
// blockDim.x must be a multiple of 32 so whole warps exit together.
// ---------------------------------------------------------------------------
__global__ void argMinKernel(const float* data, int n, int numArrays, int* out)
{
    const int warpId = (blockIdx.x * blockDim.x + threadIdx.x) >> 5;
    const int lane   = threadIdx.x & 31;
    if (warpId >= numArrays) return;

    int idx = warpArgMin64(data + (size_t)warpId * 64, n);
    if (lane == 0) out[warpId] = idx;
}

// ---------------------------------------------------------------------------
// Host test: random data, compare GPU result against a CPU reference.
// ---------------------------------------------------------------------------
#define CUDA_CHECK(x) do { cudaError_t e = (x); if (e != cudaSuccess) {        \
    fprintf(stderr, "CUDA error %s at %s:%d\n", cudaGetErrorString(e),          \
            __FILE__, __LINE__); exit(1); } } while (0)

int main()
{
    const int numArrays = 10000;
    const int n = 50;                       // try any value 1..64

    float* h_data = (float*)malloc(sizeof(float) * 64 * numArrays);
    int*   h_out  = (int*)malloc(sizeof(int) * numArrays);
    for (int i = 0; i < 64 * numArrays; ++i)
        h_data[i] = (float)rand() / RAND_MAX * 200.0f - 100.0f;

    float* d_data; int* d_out;
    CUDA_CHECK(cudaMalloc(&d_data, sizeof(float) * 64 * numArrays));
    CUDA_CHECK(cudaMalloc(&d_out,  sizeof(int) * numArrays));
    CUDA_CHECK(cudaMemcpy(d_data, h_data, sizeof(float) * 64 * numArrays,
                          cudaMemcpyHostToDevice));

    const int threads = 256;                          // 8 warps per block
    const int blocks  = (numArrays * 32 + threads - 1) / threads;
    argMinKernel<<<blocks, threads>>>(d_data, n, numArrays, d_out);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaMemcpy(h_out, d_out, sizeof(int) * numArrays,
                          cudaMemcpyDeviceToHost));

    int errors = 0;
    for (int a = 0; a < numArrays; ++a) {
        const float* arr = h_data + (size_t)a * 64;
        int ref = 0;
        for (int i = 1; i < n; ++i) if (arr[i] < arr[ref]) ref = i;
        if (ref != h_out[a]) ++errors;
    }
    printf("%s (%d mismatches out of %d arrays)\n",
           errors ? "FAILED" : "PASSED", errors, numArrays);

    cudaFree(d_data); cudaFree(d_out);
    free(h_data); free(h_out);
    return errors != 0;
}
