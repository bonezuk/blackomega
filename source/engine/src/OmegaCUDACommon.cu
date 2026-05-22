#include <cuda_runtime.h>

#include "common/inc/CommonTypes.h"
#include "engine/inc/OmegaCUDACommon.h"

//-------------------------------------------------------------------------------------------

void Omega1DCuda_ThreadDivision(int N, int& noBlocks, int& threadsPerBlock)
{
	if(N < 256)
	{
		noBlocks = 1;
		threadsPerBlock = N;
	}
	else
	{
		noBlocks = N >> 8;
		threadsPerBlock = 256;	
		if(N & 0xff)
		{
			noBlocks++;
		}
	}
}

//-------------------------------------------------------------------------------------------
// Beginning of GPU Architecture definitions
//-------------------------------------------------------------------------------------------

int _ConvertSMVer2Cores(int major, int minor) 
{
	// Defines for GPU Architecture types (using the SM version to determine
	// the # of cores per SM
	typedef struct {
		int SM;  // 0xMm (hexidecimal notation), M = SM Major version,
		// and m = SM minor version
		int Cores;
	} sSMtoCores;

	sSMtoCores nGpuArchCoresPerSM[] = {
		{0x30, 192},
		{0x32, 192},
		{0x35, 192},
		{0x37, 192},
		{0x50, 128},
		{0x52, 128},
		{0x53, 128},
		{0x60,  64},
		{0x61, 128},
		{0x62, 128},
		{0x70,  64},
		{0x72,  64},
		{0x75,  64},
		{0x80,  64},
		{0x86, 128},
		{0x87, 128},
		{0x89, 128},
		{0x90, 128},
		{0xa0, 128},
		{0xa1, 128},
		{0xa3, 128},
		{0xb0, 128},
		{0xc0, 128},
		{0xc1, 128},
		{-1, -1}
	};

	int index = 0;
	
	while (nGpuArchCoresPerSM[index].SM != -1) 
	{
		if (nGpuArchCoresPerSM[index].SM == ((major << 4) + minor)) 
		{
			return nGpuArchCoresPerSM[index].Cores;
		}
		index++;
	}

	return nGpuArchCoresPerSM[index - 1].Cores;
}

//-------------------------------------------------------------------------------------------

int initCUDAOmega()
{
    static int selectedDeviceID = -1;

	cudaError_t res;
	int currentDevice, deviceCount;
	tuint64 maxComputePerf = 0;
	int devID = -1;

    if(selectedDeviceID >= 0)
        return selectedDeviceID;
	
	deviceCount = 0;
	if(cudaGetDeviceCount(&deviceCount) != cudaSuccess)
		return -1;
	
	for(currentDevice = 0; currentDevice < deviceCount; currentDevice++)
	{
		int computeMode = -1, major = 0, minor = 0;
		int smPerMultiproc;
		
		res = cudaDeviceGetAttribute(&computeMode, cudaDevAttrComputeMode, currentDevice);
		if(res != cudaSuccess || computeMode == cudaComputeModeProhibited)
			continue;
		res = cudaDeviceGetAttribute(&major, cudaDevAttrComputeCapabilityMajor, currentDevice);
		if(res != cudaSuccess)
			continue;
		res = cudaDeviceGetAttribute(&minor, cudaDevAttrComputeCapabilityMinor, currentDevice);
		if(res != cudaSuccess)
			continue;
		
		smPerMultiproc = (major == 9999 && minor == 9999) ? 1 : _ConvertSMVer2Cores(major,  minor);
		
		int multiProcessorCount = 0, clockRate = 0;
		res = cudaDeviceGetAttribute(&multiProcessorCount, cudaDevAttrMultiProcessorCount, currentDevice);
		if(res != cudaSuccess)
			continue;
		
		res = cudaDeviceGetAttribute(&clockRate, cudaDevAttrClockRate, currentDevice);
		if(res != cudaSuccess)
		{
			if(res == cudaErrorInvalidValue)
			{
				clockRate = 1;
			}
			else
			{
				continue;
			}
		}
		
		tuint64 computePerf = static_cast<tuint64>(multiProcessorCount) * static_cast<tuint64>(smPerMultiproc) * static_cast<tuint64>(clockRate);
		if(computePerf > maxComputePerf)
		{
			maxComputePerf = computePerf;
			devID = currentDevice;
		}
	}
	if(devID >= 0)
	{
		res = cudaSetDevice(devID);
		if(res == cudaSuccess)
        {
            selectedDeviceID = devID;
        }
        else
        {
            devID = -1;
        }
	}
	return devID;
}

//-------------------------------------------------------------------------------------------
