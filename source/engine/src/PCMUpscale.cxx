#include "engine/inc/PCMUpscale.h"

#if defined(OMEGA_CUDA)
#include <cuda_runtime.h>
#include "engine/inc/OmegaCUDACommon.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

typedef struct
{
    engine::FIRFilterType type;
    int blockSize;
    int times;
} DSDFilterInfoUpscale;

//-------------------------------------------------------------------------------------------

const DSDFilterInfoUpscale c_filterDescriptionsUpscale[12] = {
    { engine::e_lpHalf_DSD0_5, 1024, 0 }, // 0
    { engine::e_lpHalf_DSD1, 2048, 1 },   // 1
    { engine::e_lpQuarter_DSD2, 4096, 2 }, // 2
    { engine::e_lpQuarter_DSD4, 8192, 4 }, // 3
    { engine::e_lpQuarter_DSD8, 16384, 8 }, // 4
    { engine::e_lpQuarter_DSD16, 32768, 16 }, // 5
    { engine::e_lpQuarter_DSD32, 65536, 32 }, // 6
    { engine::e_lpQuarter_DSD64, 131072, 64 }, // 7
    { engine::e_lpQuarter_DSD128, 262144, 128 }, // 8
    { engine::e_lpQuarter_DSD256, 524288, 256 }, // 9
    { engine::e_lpQuarter_DSD512, 1048576, 512 }, // 10
    { engine::e_lpQuarter_DSD1024, 2097152, 1024 } // 11
};

//-------------------------------------------------------------------------------------------

PCMUpscale::PCMUpscale() : m_computeMethod(PCMToDSD::e_computeMethodCPU),
    m_filterTypes(),
    m_filters(),
    m_buffers(),
    m_fbOutput(),
    m_outSHost(),
    m_inputFrequency(0),
    m_dsdTimes(0)
#if defined(OMEGA_CUDA)
    ,m_filtersCUDA(),
    m_outSDevice(NULL)
#endif
{
    if(isComputeMethodAvailable(PCMToDSD::e_computeMethodCUDA))
    {
        m_computeMethod = PCMToDSD::e_computeMethodCUDA;
    }
}

//-------------------------------------------------------------------------------------------

PCMUpscale::PCMUpscale(ComputeMethod computeMethod) : m_computeMethod(computeMethod),
    m_filterTypes(),
    m_filters(),
    m_buffers(),
    m_fbOutput(),
    m_outSHost(),
    m_inputFrequency(0),
    m_dsdTimes(0)
#if defined(OMEGA_CUDA)
    ,m_filtersCUDA(),
    m_outSDevice(NULL)
#endif
{
    if(!isComputeMethodAvailable(computeMethod))
    {
        m_computeMethod = PCMToDSD::e_computeMethodCPU;
    }
}

//-------------------------------------------------------------------------------------------

PCMUpscale::~PCMUpscale()
{
    done();
}

//-------------------------------------------------------------------------------------------

void PCMUpscale::done()
{
    m_filterTypes.clear();
    m_filters.clear();
    m_buffers.clear();
    m_fbOutput.clear();
    m_outSHost.clear();
#if defined(OMEGA_CUDA)
    for(auto& filter : m_filtersCUDA)
    {
        FIRConvAddOverlapCUDA_Free(filter);
    }
    m_filtersCUDA.clear();
    if(m_outSDevice != NULL)
    {
        cudaFree(m_outSDevice);
        m_outSDevice = NULL;
    }
#endif
}

//-------------------------------------------------------------------------------------------

bool PCMUpscale::isComputeMethodAvailable(ComputeMethod method)
{
    return PCMToDSD::isComputeMethodAvailable(method);
}

//-------------------------------------------------------------------------------------------

PCMUpscale::ComputeMethod PCMUpscale::computeMethod() const
{
    return m_computeMethod;
}

//-------------------------------------------------------------------------------------------

int PCMUpscale::noInputSamples() const
{
    int num = -1;
    engine::FIRFilterType type;

    type = filterTypeAtIndex(0);
    if(type != engine::e_NoFilter)
    {
        int idx = filterIndexOfType(type);
        if(idx >= 0)
        {
            num = c_filterDescriptionsUpscale[idx].blockSize >> 1;
        }
    }
    return num;
}

//-------------------------------------------------------------------------------------------

int PCMUpscale::noOutputSamples() const
{
    int num = -1;
    engine::FIRFilterType type;

    type = filterTypeAtIndex(m_filterTypes.size() - 1);
    if(type != engine::e_NoFilter)
    {
        int idx = filterIndexOfType(type);
        if(idx >= 0)
        {
            num = c_filterDescriptionsUpscale[idx].blockSize;
        }
    }
    return num;
}

//-------------------------------------------------------------------------------------------

int PCMUpscale::noSteps(int N) const
{
	int count = 0;

	while(N > 1)
	{
		N >>= 1;
		count++;
	}
	return count;
}

//-------------------------------------------------------------------------------------------

engine::FIRFilterType PCMUpscale::filterForFrequency(int freq) const
{
    engine::FIRFilterType type = engine::e_NoFilter;

    if(freq == 11025 || freq == 12000)
    {
        type = engine::e_lpHalf_DSD0_5;
    }
    else if(freq == 22050 || freq == 24000)
    {
        type = engine::e_lpHalf_DSD1;
    }
    else if(freq == 44100 || freq == 48000)
    {
        type = engine::e_lpHalf_DSD2;
    }
    else if(freq == 88200 || freq == 96000)
    {
        type = engine::e_lpHalf_DSD4;
    }
    else if(freq == 176400 || freq == 192000)
    {
        type = engine::e_lpHalf_DSD8;
    }
    else if(freq == 352800 || freq == 384000)
    {
        type = engine::e_lpQuarter_DSD16;
    }
    else if(freq == 705600 || freq == 768000)
    {
        type = engine::e_lpQuarter_DSD32;
    }
    return type;
}

//-------------------------------------------------------------------------------------------

engine::FIRFilterType PCMUpscale::filterTypeAtIndex(int idx) const
{
    engine::FIRFilterType type = engine::e_NoFilter;

    if(idx >= 0 && idx < m_filterTypes.size())
    {
        type = m_filterTypes.at(idx);
    }
    return type;
}

//-------------------------------------------------------------------------------------------

int PCMUpscale::filterIndexOfType(engine::FIRFilterType type) const
{
    int idx;

    switch(type)
    {
        case engine::e_lpHalf_DSD0_5:
            idx = 0;
            break;
        case engine::e_lpHalf_DSD1:
            idx = 1;
            break;
        case engine::e_lpHalf_DSD2:
        case engine::e_lpQuarter_DSD2:
            idx = 2;
            break;
        case engine::e_lpHalf_DSD4:
        case engine::e_lpQuarter_DSD4:
            idx = 3;
            break;
        case engine::e_lpHalf_DSD8:
        case engine::e_lpQuarter_DSD8:
            idx = 4;
            break;
        case engine::e_lpQuarter_DSD16:
            idx = 5;
            break;
        case engine::e_lpQuarter_DSD32:
            idx = 6;
            break;
        case engine::e_lpQuarter_DSD64:
            idx = 7;
            break;
        case engine::e_lpQuarter_DSD128:
            idx = 8;
            break;
        case engine::e_lpQuarter_DSD256:
            idx = 9;
            break;
        case engine::e_lpQuarter_DSD512:
            idx = 10;
            break;
        case engine::e_lpQuarter_DSD1024:
            idx = 11;
            break;
        default:
            idx = -1;
            break;
    }
    return idx;
}

//-------------------------------------------------------------------------------------------

bool PCMUpscale::init(int inputFrequency, int dsdTimes)
{
    bool res;

    done();

    int steps = noSteps(dsdTimes);

    if(dsdTimes != (1 << steps))
        return false;

    int idx;
    engine::FIRFilterType startType = filterForFrequency(inputFrequency);
    idx = filterIndexOfType(startType);
    if(idx < 0)
        return false;
    if(c_filterDescriptionsUpscale[idx].times >= dsdTimes)
        return false;

    QVector<engine::FIRFilterType> filters;
    filters.append(startType);
    idx++;
    while(idx < 12 && c_filterDescriptionsUpscale[idx].times <= dsdTimes)
    {
        filters.append(c_filterDescriptionsUpscale[idx].type);
        idx++;
    }

    res = true;
    for(auto ppI = filters.begin(); ppI != filters.end() && res; ppI++)
    {
        int coeffLen;
        const engine::FIRFilterType& type = *ppI;

        res = false;
        idx = filterIndexOfType(type);
        if(idx >= 0 && idx <12)
        {
            double *coeff = getFIRFilterFromDB(type, coeffLen);

            if(coeff != NULL)
            {
                if(m_computeMethod == PCMToDSD::e_computeMethodCPU)
                {
                    QSharedPointer<FIRConvolutionAddOverlapOctaveUpscale> pFilter(new FIRConvolutionAddOverlapOctaveUpscale());
                    if(pFilter->init(coeff, coeffLen, c_filterDescriptionsUpscale[idx].blockSize))
                    {
                        m_filters.append(pFilter);
                        res = true;
                    }
                }
#if defined(OMEGA_CUDA)
                else if(m_computeMethod == PCMToDSD::e_computeMethodCUDA)
                {
                    FIRConvAddOverlapCuda_Data *pFilter = FIRConvAddOverlapCUDA_Init(coeff, coeffLen, c_filterDescriptionsUpscale[idx].blockSize);
                    if(pFilter != NULL)
                    {
                        m_filtersCUDA.append(pFilter);
                        res = true;
                    }
                }
#endif
                if(res)
                {
                    m_filterTypes.append(type);
                }
                delete [] coeff;
            }
        }
    }

    if(res)
    {
        if(m_computeMethod == PCMToDSD::e_computeMethodCPU)
        {
            // Each filter bar the last writes to its own buffer, which is the input to the next.
            for(int i = 0; i < m_filterTypes.size() - 1; i++)
            {
                idx = filterIndexOfType(m_filterTypes.at(i));
                QSharedPointer<double> pBuffer(new double [c_filterDescriptionsUpscale[idx].blockSize], [](double *p) { delete [] p; });
                m_buffers.append(pBuffer);
            }
            QSharedPointer<float> pOutS(new float [noOutputSamples()], [](float *p) { delete [] p; });
            m_outSHost = pOutS;
        }
#if defined(OMEGA_CUDA)
        else if(m_computeMethod == PCMToDSD::e_computeMethodCUDA)
        {
            if(cudaMalloc(&m_outSDevice, sizeof(float) * noOutputSamples()) != cudaSuccess)
            {
                m_outSDevice = NULL;
                res = false;
            }
        }
#endif
        else
        {
            res = false;
        }
    }

    if(res)
    {
        QSharedPointer<double> pBuffer(new double [noOutputSamples()], [](double *p) { delete [] p; });
        m_fbOutput = pBuffer;
        m_inputFrequency = inputFrequency;
        m_dsdTimes = dsdTimes;
    }
    else
    {
        done();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void PCMUpscale::processFilterBankCPU(const double *in, double *out)
{
    int idx;
    const double *x;
    double *X;

    for(idx = 0; idx < m_filters.size(); idx++)
    {
        x = (idx == 0) ? in : m_buffers.at(idx - 1).get();
        X = (idx < m_filters.size() - 1) ? m_buffers.at(idx).get() : out;
        m_filters.at(idx)->process(x, X);
    }
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_CUDA)
//-------------------------------------------------------------------------------------------
// Run the filter bank leaving the output in device memory owned by the last filter.

const double *PCMUpscale::processFilterBankOnDeviceCUDA(const double *in)
{
    const double *X = in;

    for(int idx = 0; idx < m_filtersCUDA.size() && X != NULL; idx++)
    {
        X = FIRConvAddOverlapCUDA_OctaveUpscale_Process_Device(X, m_filtersCUDA.at(idx), (!idx) ? true : false);
    }
    return X;
}

//-------------------------------------------------------------------------------------------

bool PCMUpscale::processFilterBankCUDA(const double *in, double *out)
{
    const double *X = processFilterBankOnDeviceCUDA(in);
    if(X == NULL)
        return false;
    if(cudaMemcpy(out, X, sizeof(double) * noOutputSamples(), cudaMemcpyDeviceToHost) != cudaSuccess)
        return false;
    return true;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

bool PCMUpscale::processFilterBank(const double *in, double *out)
{
#if defined(OMEGA_CUDA)
    if(m_computeMethod == PCMToDSD::e_computeMethodCUDA)
    {
        return processFilterBankCUDA(in, out);
    }
#endif
    processFilterBankCPU(in, out);
    return true;
}

//-------------------------------------------------------------------------------------------

int PCMUpscale::outputFrequency() const
{
    return m_inputFrequency * m_dsdTimes;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<double> PCMUpscale::upscale(const double *in, int noInput, int& noOutput)
{
    const int noIn = noInputSamples();
    const int noOut = noOutputSamples();

    if(in == NULL || noIn <= 0 || noOut <= 0 || noInput <= 0 || (noInput % noIn) != 0)
    {
        return QSharedPointer<double>();
    }

    int noBlocks = noInput / noIn;
    noOutput = noOut * noBlocks;
    QSharedPointer<double> pBuffer(new double [noOutput], [](double *p) { delete [] p; });

    bool res = true;
    double *out = pBuffer.get();
    for(int blockIdx = 0; blockIdx < noBlocks && res; blockIdx++)
    {
        res = processFilterBank(in, out);
        in += noIn;
        out += noOut;
    }
    if(!res)
    {
        pBuffer.clear();
    }
    return pBuffer;
}

//-------------------------------------------------------------------------------------------
// The upscaled block is returned in memory local to where it was computed: GPU device memory
// for CUDA, host memory for the CPU.

const float *PCMUpscale::upscaleSingleOnDevice(const double *in, int noInput, int& noOutput)
{
    const int noOut = noOutputSamples();

    if(in == NULL || noOut <= 0 || noInput != noInputSamples())
    {
        return NULL;
    }

#if defined(OMEGA_CUDA)
    if(m_computeMethod == PCMToDSD::e_computeMethodCUDA)
    {
        const double *X = processFilterBankOnDeviceCUDA(in);
        if(X == NULL)
        {
            return NULL;
        }
        noOutput = noOut;
        convertCUDADoubleToFloat(X, m_outSDevice, noOut);
        return m_outSDevice;
    }
#endif

    const double *X = m_fbOutput.get();
    float *out = m_outSHost.get();
    processFilterBankCPU(in, m_fbOutput.get());
    for(int idx = 0; idx < noOut; idx++)
    {
        out[idx] = static_cast<float>(X[idx]);
    }
    noOutput = noOut;
    return out;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
