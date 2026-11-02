#include <cuda_runtime.h>

#include "engine/inc/PCMUpscale.h"

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

PCMUpscale::PCMUpscale() : m_outSDevice(NULL)
{}

//-------------------------------------------------------------------------------------------

PCMUpscale::~PCMUpscale()
{}

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
    
    type = filterTypeAtIndex(m_filtersCUDA.size() - 1);
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

    if(idx >= 0 && idx < m_filtersCUDA.size())
    {
        type = m_filtersCUDA.at(idx).first;
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
                FIRConvAddOverlapCuda_Data *pFilter = FIRConvAddOverlapCUDA_Init(coeff, coeffLen, c_filterDescriptionsUpscale[idx].blockSize);
                if(pFilter != NULL)
                {
                    m_filtersCUDA.append(qMakePair(type, pFilter));
                    res = true;
                }
                delete [] coeff;
            }
        }
    }
    if(filters.isEmpty())
    {
        res = false;
    }
    
    if(res)
    {
    	if(cudaMalloc(&m_outSDevice, sizeof(float) * noOutputSamples()) != cudaSuccess)
    	{
    		res = false;
    	}
    }

    if(res)
    {
        idx = filterIndexOfType(m_filtersCUDA.at(m_filtersCUDA.size() - 1).first);
        QSharedPointer<double> pBuffer(new double [c_filterDescriptionsUpscale[idx].blockSize]);
        m_fbOutput = pBuffer;
        m_inputFrequency = inputFrequency;
        m_dsdTimes = dsdTimes;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool PCMUpscale::processFilterBankCUDA(const double *in, double *out)
{
    int idx;
    const double *X;

    X = in;
    for(idx = 0; idx < m_filtersCUDA.size(); idx++)
    {
        X = FIRConvAddOverlapCUDA_OctaveUpscale_Process_Device(X, m_filtersCUDA.at(idx).second, (!idx) ? true : false);
        if(X == NULL)
            return false;
    }
    if(cudaMemcpy(out, X, sizeof(double) * noOutputSamples(), cudaMemcpyDeviceToHost) != cudaSuccess)
        return false;
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
    if(in == NULL || (noInput % noInputSamples()) != 0)
    {
        return QSharedPointer<double>();
    }

    int noBlocks = noInput / noInputSamples();
    int idx = filterIndexOfType(m_filtersCUDA.at(m_filtersCUDA.size() - 1).first);
    noOutput = c_filterDescriptionsUpscale[idx].blockSize * noBlocks;
    QSharedPointer<double> pBuffer(new double [noOutput]);

    bool res = true;
    double *out = pBuffer.get();
    for(int idx = 0; idx < noBlocks && res; idx++)
    {
        res = processFilterBankCUDA(in, out);
        if(res)
        {
            in += noInputSamples();
            out += c_filterDescriptionsUpscale[idx].blockSize;
        }
    }
    if(!res)
    {
        pBuffer.clear();
    }
    return pBuffer;
}

//-------------------------------------------------------------------------------------------

const float *PCMUpscale::upscaleSingleOnDevice(const double *in, int noInput, int& noOutput)
{
	const double *X;
	
	if(in == NULL || noInput == noInputSamples())
	{
		return NULL;
	}

    X = in;
    for(idx = 0; idx < m_filtersCUDA.size(); idx++)
    {
        X = FIRConvAddOverlapCUDA_OctaveUpscale_Process_Device(X, m_filtersCUDA.at(idx).second, (!idx) ? true : false);
        if(X == NULL)
        {
        	return NULL;
        }
    }
    noOutput = noOutputSamples();
    convertCUDADoubleToFloat(X, m_outSDevice, noOutput);
	return m_outSDevice;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
