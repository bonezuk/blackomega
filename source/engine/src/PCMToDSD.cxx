#include "engine/inc/PCMToDSD.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

typedef struct
{
    FIRFilterType type;
    int blockSize;
    int times;
} DSDFilterInfo;

const DSDFilterInfo c_filterDescriptions[12] = {
    { e_lpHalf_DSD0_5, 1024, 0 }, // 0
    { e_lpHalf_DSD1, 2048, 1 },   // 1
    { e_lpQuarter_DSD2, 4096, 2 }, // 2
    { e_lpQuarter_DSD4, 8192, 4 }, // 3
    { e_lpQuarter_DSD8, 16384, 8 }, // 4
    { e_lpQuarter_DSD16, 32768, 16 }, // 5
    { e_lpQuarter_DSD32, 65536, 32 }, // 6
    { e_lpQuarter_DSD64, 131072, 64 }, // 7
    { e_lpQuarter_DSD128, 262144, 128 }, // 8
    { e_lpQuarter_DSD256, 524288, 256 }, // 9
    { e_lpQuarter_DSD512, 1048576, 512 }, // 10
    { e_lpQuarter_DSD1024, 2097152, 1024 } // 11
};

//-------------------------------------------------------------------------------------------


PCMToDSD::PCMToDSD() : m_filters(),
    m_buffers(),
    m_modulator(),
    m_inputFrequency(0),
    m_dsdTimes(0)
{}

//-------------------------------------------------------------------------------------------

PCMToDSD::~PCMToDSD()
{}

//-------------------------------------------------------------------------------------------

int PCMToDSD::filterIndexOfType(FIRFilterType type) const
{
    int idx;

    switch(type)
    {
        case e_lpHalf_DSD0_5:
            idx = 0;
            break;
        case e_lpHalf_DSD1:
            idx = 1;
            break;
        case e_lpHalf_DSD2:
        case e_lpQuarter_DSD2:
            idx = 2;
            break;
        case e_lpHalf_DSD4:
        case e_lpQuarter_DSD4:
            idx = 3;
            break;
        case e_lpHalf_DSD8:
        case e_lpQuarter_DSD8:
            idx = 4;
            break;
        case e_lpQuarter_DSD16:
            idx = 5;
            break;
        case e_lpQuarter_DSD32:
            idx = 6;
            break;
        case e_lpQuarter_DSD64:
            idx = 7;
            break;
        case e_lpQuarter_DSD128:
            idx = 8;
            break;
        case e_lpQuarter_DSD256:
            idx = 9;
            break;
        case e_lpQuarter_DSD512:
            idx = 10;
            break;
        case e_lpQuarter_DSD1024:
            idx = 11;
            break;
        default:
            idx = -1;
            break;
    }
    return idx;
}

//-------------------------------------------------------------------------------------------

bool PCMToDSD::init(int inputFrequency, int dsdTimes, bool isLSB)
{
    bool res;

    int steps = noSteps(dsdTimes);

    if(dsdTimes != (1 << steps))
        return false;
    
    int idx;
    FIRFilterType startType = filterForFrequency(inputFrequency);
    idx = filterIndexOfType(startType);
    if(idx < 0)
        return false;
    if(c_filterDescriptions[idx].times >= dsdTimes)
        return false;
    
    QVector<FIRFilterType> filters;
    filters.append(startType);
    idx++;
    while(idx < 12 && c_filterDescriptions[idx].times <= dsdTimes)
    {
        filters.append(c_filterDescriptions[idx].type);
        idx++;
    }

    res = true;
    for(auto ppI = filters.begin(); ppI != filters.end() && res; ppI++)
    {
        int coeffLen;
        const FIRFilterType& type = *ppI;

        idx = filterIndexOfType(type);
        if(idx >= 0 && idx <12)
        {
            double *coeff = getFIRFilterFromDB(type, coeffLen);
            if(coeff != NULL)
            {
                QSharedPointer<FIRConvolutionAddOverlapOctaveUpscale> pFilter(new FIRConvolutionAddOverlapOctaveUpscale());
                if(pFilter->init(coeff, coeffLen, c_filterDescriptions[idx].blockSize))
                {
                    m_filters.append(qMakePair(type, pFilter));
                }
                else
                {
                    res = false;
                }
                delete [] coeff;
            }
        }
        else
        {
            res = false;
        }
    }
    if(m_filters.isEmpty())
    {
        res = false;
    }

    if(res)
    {
        for(const auto& filter : m_filters)
        {
            idx = filterIndexOfType(filter.first);
            QSharedPointer<double> pBuffer(new double [c_filterDescriptions[idx].blockSize]);
            m_buffers.append(pBuffer);
        }

        m_modulator.init(isLSB);

        m_inputFrequency = inputFrequency;
        m_dsdTimes = dsdTimes;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

int PCMToDSD::noSteps(int N) const
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

int PCMToDSD::baseFrequency(int freq) const
{
    int bFreq = 0;

    if(freq < 44100)
    {
        if(!(44100 % freq))
        {
            bFreq = 44100;
        }
        else if(!(48000 % freq))
        {
            bFreq = 48000;
        }
    }
    else
    {
        if(!(freq % 44100))
        {
            bFreq = 44100;
        }
        else if(!(freq % 48000))
        {
            bFreq = 48000;
        }
    }
    return bFreq;
}

//-------------------------------------------------------------------------------------------

FIRFilterType PCMToDSD::filterForFrequency(int freq) const
{
    FIRFilterType type = e_NoFilter;

    if(freq == 11025 || freq == 12000)
    {
        type = e_lpHalf_DSD0_5;
    }
    else if(freq == 22050 || freq == 24000)
    {
        type = e_lpHalf_DSD1;
    }
    else if(freq == 44100 || freq == 48000)
    {
        type = e_lpHalf_DSD2;
    }
    else if(freq == 88200 || freq == 96000)
    {
        type = e_lpHalf_DSD4;
    }
    else if(freq == 176400 || freq == 192000)
    {
        type = e_lpHalf_DSD8;
    }
    else if(freq == 352800 || freq == 384000)
    {
        type = e_lpQuarter_DSD16;
    }
    else if(freq == 705600 || freq == 768000)
    {
        type = e_lpQuarter_DSD32;
    }
    return type;
}

//-------------------------------------------------------------------------------------------

void PCMToDSD::process(const double *in, uint8_t *out)
{
    int idx;
    const double *x;
    double *X;

    for(idx = 0; idx < m_filters.size(); idx++)
    {
        x = (idx == 0) ? in : m_buffers.at(idx - 1).get();
        X = m_buffers.at(idx).get();
        m_filters.at(idx).second->process(x, X);
    }

    X = m_buffers.at(m_buffers.size() - 1).get();
    m_modulator.process(X, out, noOutputSamples());
}

//-------------------------------------------------------------------------------------------

bool PCMToDSD::isLSB() const
{
    return m_modulator.isLSB();
}

//-------------------------------------------------------------------------------------------

int PCMToDSD::noInputSamples() const
{
    int num = -1;
    if(!m_filters.isEmpty())
    {
        int idx = filterIndexOfType(m_filters.at(0).first);
        if(idx >= 0)
        {
            num = c_filterDescriptions[idx].blockSize >> 1;
        }
    }
    return num;
}

//-------------------------------------------------------------------------------------------

int PCMToDSD::noOutputSamples() const
{
    int num = -1;
    if(!m_filters.isEmpty())
    {
        int idx = filterIndexOfType(m_filters.at(m_filters.size() - 1).first);
        if(idx >= 0)
        {
            num = c_filterDescriptions[idx].blockSize;
        }
    }
    return num;
}

//-------------------------------------------------------------------------------------------

int PCMToDSD::noOutputBytes() const
{
    return noOutputSamples() >> 3;
}

//-------------------------------------------------------------------------------------------

int PCMToDSD::inputFrequency() const
{
    return m_inputFrequency;
}

//-------------------------------------------------------------------------------------------

int PCMToDSD::outputFrequency() const
{
    return baseFrequency(m_inputFrequency) * m_dsdTimes;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

