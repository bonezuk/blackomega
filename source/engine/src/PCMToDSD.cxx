#include "engine/inc/PCMToDSD.h"
#include "engine/inc/FormatType.h"

#if defined(OMEGA_CUDA)
#include <cuda_runtime.h>
#endif

#if !defined(OMEGA_WIN32)
#include <pthread.h>
#endif

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

PCMToDSD::PCMToDSD(): m_computeMethod(e_computeMethodCPU),
    m_dataType(e_SampleDSD8LSB),
    m_filters(),
    m_buffers(),
    m_modulator(),
    m_inputFrequency(0),
    m_dsdTimes(0),
#if defined(OMEGA_CUDA)
    m_filtersCUDA(),
    m_fbOutput(),
#endif
    m_available(0),
    m_noChannels(1),
    m_channelIndex(0),
    m_noInputSamples(0),
    m_inputBufferAmount(0),
    m_inputBuffer(NULL),
    m_outputBufferAmount(0),
    m_outputBuffer(NULL),
    m_inputQueue(),
    m_dsmQueue(),
    m_outputQueue(),
    m_isRunning(false),
    m_markerInc(0)
{
    if(isComputeMethodAvailable(e_computeMethodCUDA))
    {
        m_computeMethod = e_computeMethodCUDA;
    }
    for(int idx = 0; idx < 2; idx++)
    {
#if defined(OMEGA_WIN32)
        m_hThreads[idx] = NULL;
#else
        m_threadIDs[idx] = 0;
#endif
    }
}

//-------------------------------------------------------------------------------------------

PCMToDSD::PCMToDSD(ComputeMethod computeMethod) : m_computeMethod(computeMethod),
    m_dataType(e_SampleDSD8LSB),
    m_filters(),
    m_buffers(),
    m_modulator(),
    m_inputFrequency(0),
    m_dsdTimes(0),
#if defined(OMEGA_CUDA)
	m_filtersCUDA(),
	m_fbOutput(),
#endif
	m_available(0),
	m_noChannels(1),
	m_channelIndex(0),
	m_noInputSamples(0),
	m_inputBufferAmount(0),
	m_inputBuffer(NULL),
	m_outputBufferAmount(0),
	m_outputBuffer(NULL),
	m_inputQueue(),
	m_dsmQueue(),
	m_outputQueue(),
	m_isRunning(false),
    m_markerInc(0)
{
    if(!isComputeMethodAvailable(computeMethod))
    {
        m_computeMethod = e_computeMethodCPU;
    }
	for(int idx = 0; idx < 2; idx++)
	{
#if defined(OMEGA_WIN32)
		m_hThreads[idx] = NULL;
#else
		m_threadIDs[idx] = 0;
#endif
	}
}

//-------------------------------------------------------------------------------------------

PCMToDSD::~PCMToDSD()
{
    done();
}

//-------------------------------------------------------------------------------------------

bool PCMToDSD::isComputeMethodAvailable(ComputeMethod method)
{
    bool isAvailable = false;

    if(method == e_computeMethodCPU)
    {
        isAvailable = true;
    }
#if defined(OMEGA_CUDA)
    else if(method == e_computeMethodCUDA)
    {
        if(initCUDAOmega() >= 0)
        {
            isAvailable = true;
        }
    }
#endif
    return isAvailable;
}

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

void PCMToDSD::done()
{
    stopThreads();

    if(m_inputBuffer != NULL)
    {
        m_inputQueue->free(m_inputBuffer);
        m_inputBuffer = 0;
    }
    m_inputBufferAmount = 0;
    if(m_outputBuffer != NULL)
    {
        m_outputQueue->free(m_outputBuffer);
        m_outputBuffer = 0;
    }
    m_outputBufferAmount = 0;
    
    m_inputQueue.clear();
    m_dsmQueue.clear();
    m_outputQueue.clear();

    m_filters.clear();
    m_buffers.clear();
#if defined(OMEGA_CUDA)
    for(auto &filter : m_filtersCUDA)
    {
        FIRConvAddOverlapCUDA_Free(filter.second);
    }
    m_fbOutput.clear();
#endif
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

        res = false;
        idx = filterIndexOfType(type);
        if(idx >= 0 && idx <12)
        {            
            double *coeff = getFIRFilterFromDB(type, coeffLen);

            if(coeff != NULL)
            {
                if(m_computeMethod == e_computeMethodCPU)
                {
                    QSharedPointer<FIRConvolutionAddOverlapOctaveUpscale> pFilter(new FIRConvolutionAddOverlapOctaveUpscale());
                    if(pFilter->init(coeff, coeffLen, c_filterDescriptions[idx].blockSize))
                    {
                        m_filters.append(qMakePair(type, pFilter));
                        res = true;
                    }
                }
#if defined(OMEGA_CUDA)
                else if(m_computeMethod == e_computeMethodCUDA)
                {
                    FIRConvAddOverlapCuda_Data *pFilter = FIRConvAddOverlapCUDA_Init(coeff, coeffLen, c_filterDescriptions[idx].blockSize);
                    if(pFilter != NULL)
                    {
                        m_filtersCUDA.append(qMakePair(type, pFilter));
                        res = true;
                    }
                }
#endif
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
        if(m_computeMethod == e_computeMethodCPU)
        {
            for(const auto& filter : m_filters)
            {
                idx = filterIndexOfType(filter.first);
                QSharedPointer<double> pBuffer(new double [c_filterDescriptions[idx].blockSize]);
                m_buffers.append(pBuffer);
            }
        }
#if defined(OMEGA_CUDA)
        else if(m_computeMethod == e_computeMethodCUDA)
        {
            idx = filterIndexOfType(m_filtersCUDA.at(m_filtersCUDA.size() - 1).first);
            QSharedPointer<double> pBuffer(new double [c_filterDescriptions[idx].blockSize]);
            m_fbOutput = pBuffer;
        }
#endif
        else
        {
            res = false;
        }

        m_modulator.init(isLSB);
        m_dataType = (isLSB) ? e_SampleDSD8LSB : e_SampleDSD8MSB;
        m_noChannels = 1;
        m_markerInc = 0;

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

bool PCMToDSD::process(const double *in, uint8_t *out)
{
    double *X;

#if defined(OMEGA_CUDA)
    if(m_computeMethod == e_computeMethodCUDA)
    {
        X = m_fbOutput.get();
        if(!processFilterBankCUDA(in, X))
        {
            X = NULL;
        }
    }
    else
#endif
    {
        X = m_buffers.at(m_filters.size() - 1).get();
        processFilterBankCPU(in, X);
    }
    if(X == NULL)
    {
        return false;
    }
    m_modulator.process(X, out, noOutputSamples());
    return true;
}

//-------------------------------------------------------------------------------------------

void PCMToDSD::processFilterBankCPU(const double *in, double *out)
{
    int idx;
    const double *x;
    double *X;

    for(idx = 0; idx < m_filters.size(); idx++)
    {
        x = (idx == 0) ? in : m_buffers.at(idx - 1).get();
        X = (idx < m_filters.size() - 1) ? m_buffers.at(idx).get() : out;
        m_filters.at(idx).second->process(x, X);
    }
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_CUDA)
//-------------------------------------------------------------------------------------------

bool PCMToDSD::processFilterBankCUDA(const double *in, double *out)
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
#endif
//-------------------------------------------------------------------------------------------

bool PCMToDSD::isLSB() const
{
    return m_modulator.isLSB();
}

//-------------------------------------------------------------------------------------------

int PCMToDSD::noFilters() const
{
    int num = 0;
#if defined(OMEGA_CUDA)
    if(m_computeMethod == e_computeMethodCUDA)
    {
        num = m_filtersCUDA.size();
    }
    else
#endif
    {
        num = m_filters.size();
    }
    return num;
}

//-------------------------------------------------------------------------------------------

FIRFilterType PCMToDSD::filterTypeAtIndex(int idx) const
{
    FIRFilterType type = e_NoFilter;

#if defined(OMEGA_CUDA)
    if(m_computeMethod == e_computeMethodCUDA)
    {
        if(idx >= 0 && idx < m_filtersCUDA.size())
        {
            type = m_filtersCUDA.at(idx).first;
        }
    }
    else
#endif
    {
        if(idx >= 0 && idx < m_filters.size())
        {
            type = m_filters.at(idx).first;
        }
    }
    return type;
}

//-------------------------------------------------------------------------------------------

int PCMToDSD::noInputSamples() const
{
    int num = -1;
    FIRFilterType type;
    
    type = filterTypeAtIndex(0);
    if(type != e_NoFilter)
    {
        int idx = filterIndexOfType(type);
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

    FIRFilterType type;
    
    type = filterTypeAtIndex(noFilters() - 1);
    if(type != e_NoFilter)
    {
        int idx = filterIndexOfType(type);
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
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

DWORD WINAPI PCMToDSD::filterBankThread(LPVOID arg)
{
    PCMToDSD *pInstance = reinterpret_cast<PCMToDSD *>(arg);
    pInstance->filterBankMain();
    return 0;
}

DWORD WINAPI PCMToDSD::deltaSigmaThread(LPVOID arg)
{
    PCMToDSD *pInstance = reinterpret_cast<PCMToDSD *>(arg);
    pInstance->deltaSigmaMain();
    return 0;
}

//-------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------

void *PCMToDSD::filterBankThread(void *arg)
{
    PCMToDSD *pInstance = reinterpret_cast<PCMToDSD *>(arg);
    pInstance->filterBankMain();
    return 0;
}

void *PCMToDSD::deltaSigmaThread(void *arg)
{
    PCMToDSD *pInstance = reinterpret_cast<PCMToDSD *>(arg);
    pInstance->deltaSigmaMain();
    return 0;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

int PCMToDSD::available() const
{
    int avail = m_available;
    if(m_outputBuffer != NULL)
    {
        avail += m_outputQueue->arraySize() - m_outputBufferAmount;
    }
    return avail;
}

//-------------------------------------------------------------------------------------------

int PCMToDSD::numberOfDSDBytesInAPCMSample() const
{
    int num;

    if(m_dataType == e_SampleDSD8LSB || m_dataType == e_SampleDSD8MSB)
    {
        num = sizeof(sample_t);
    }
    else
    {
        num = 2;
    }
    return num;
}

//-------------------------------------------------------------------------------------------

int PCMToDSD::availablePCMSamples() const
{
    return available() / numberOfDSDBytesInAPCMSample();
}

//-------------------------------------------------------------------------------------------

CodecDataType PCMToDSD::dataType() const
{
    return m_dataType;
}

//-------------------------------------------------------------------------------------------

int PCMToDSD::push(const sample_t *in, int noSamples)
{
    if(in == NULL || noSamples <= 0)
        return 0;

    if(m_inputBuffer == NULL)
    {
        m_inputBuffer = m_inputQueue->get();
        m_inputBufferAmount = 0;
    }

    int pos = 0;
    while(pos < noSamples * m_noChannels)
    {
        m_inputBuffer[m_inputBufferAmount] = in[pos + m_channelIndex];
        m_inputBufferAmount++;
        pos += m_noChannels;

        if(m_inputBufferAmount >= m_noInputSamples)
        {
            m_inputQueue->push(m_inputBuffer);
            m_inputBuffer = m_inputQueue->get();
            m_inputBufferAmount = 0;
            m_available += m_outputQueue->arraySize();
        }
    }
    return (pos / m_noChannels);
}

//-------------------------------------------------------------------------------------------

int PCMToDSD::pull(uint8_t *out, int noBytes)
{
    if(out == NULL || noBytes <= 0)
        return 0;
    
    int pos = 0;
    while(available() > 0 && pos < noBytes)
    {
        if(m_outputBuffer == NULL)
        {
            m_outputBuffer = m_outputQueue->pull();
            m_outputBufferAmount = 0;
            m_available -= m_outputQueue->arraySize();
        }

        int amountI = m_outputQueue->arraySize() - m_outputBufferAmount;
        int amountO = noBytes - pos;
        int amount = (amountI < amountO) ? amountI : amountO;
        memcpy(&out[pos], &m_outputBuffer[m_outputBufferAmount], amount * sizeof(uint8_t));
        m_outputBufferAmount += amount;
        pos += amount;

        if(m_outputBufferAmount >=  m_outputQueue->arraySize())
        {
            m_outputQueue->free(m_outputBuffer);
            m_outputBuffer = NULL;
        }
    }
    return pos;
}

//-------------------------------------------------------------------------------------------

void PCMToDSD::filterBankMain()
{
    double *in, *out;

    while(m_isRunning)
    {
        in = m_inputQueue->pull(50);
        if(in != NULL)
        {
            out = m_dsmQueue->get();

#if defined(OMEGA_CUDA)
            if(m_computeMethod == e_computeMethodCUDA)
            {
                Q_ASSERT(processFilterBankCUDA(in, out));
            }
            else
#endif
            {
                processFilterBankCPU(in, out);
            }

            m_dsmQueue->push(out);
            m_inputQueue->free(in);
        }
    }
}

//-------------------------------------------------------------------------------------------

void PCMToDSD::deltaSigmaMain()
{
    uint8_t *out;
    double *in;

    while(m_isRunning)
    {
        in = m_dsmQueue->pull(50);
        if(in != NULL)
        {
            out = m_outputQueue->get();
            m_modulator.process(in, out, noOutputSamples());
            m_outputQueue->push(out);
            m_dsmQueue->free(in);
        }
    }
}

//-------------------------------------------------------------------------------------------

bool PCMToDSD::startThreads()
{
    bool res = false;

    stopThreads();

    m_isRunning = true;
#if defined(OMEGA_WIN32)
    DWORD ids[2];
    m_hThreads[0] = CreateThread(0, 0, PCMToDSD::filterBankThread, reinterpret_cast<LPVOID>(this), 0, &ids[0]);
    if(m_hThreads[0] != NULL)
    {
        m_hThreads[1] = CreateThread(0, 0, PCMToDSD::deltaSigmaThread, reinterpret_cast<LPVOID>(this), 0, &ids[1]);
        if(m_hThreads[1] != NULL)
        {
            res = true;
        }
    }
#else
    if(!pthread_create(&m_threadIDs[0], 0, PCMToDSD::filterBankThread, this))
    {
        if(!pthread_create(&m_threadIDs[1], 0, PCMToDSD::deltaSigmaThread, this))
        {
            res = true;
        }
    }
#endif
    return res;
}

//-------------------------------------------------------------------------------------------

void PCMToDSD::stopThreads()
{
    m_isRunning = false;
#if defined(OMEGA_WIN32)
    for(int idx = 0; idx < 2; idx++)
    {
        if(m_hThreads[idx] != NULL)
        {
            WaitForSingleObject(m_hThreads[idx], INFINITE);
            CloseHandle(m_hThreads[idx]);
            m_hThreads[idx] = NULL;
        }
    }
#else
    for(int idx = 0; idx < 1; idx++)
    {
        if(m_threadIDs[idx] != 0)
        {
            pthread_join(m_threadIDs[idx], NULL);
            m_threadIDs[idx] = 0;
        }
    }
#endif
}

//-------------------------------------------------------------------------------------------

bool PCMToDSD::initInterleaved(int inputFrequency, int dsdTimes, CodecDataType dataType, int channelIndex, int noChannels)
{
    if(dataType == e_SampleFloat || dataType == e_SampleInt16)
        return false;
    
    bool isLSB = (dataType == e_SampleDSD8LSB) ? true : false;
    if(!init(inputFrequency, dsdTimes, isLSB))
        return false;
    
    m_dataType = dataType;
    m_available = 0;
    m_noChannels = noChannels;
    m_channelIndex = channelIndex;
    m_noInputSamples = noInputSamples();
    m_inputBufferAmount = 0;
    m_inputBuffer = NULL;
    m_outputBufferAmount = 0;
    m_outputBuffer = NULL;

    QSharedPointer<FixedDataQueue<double> > inputQueue(new FixedDataQueue<double>(m_noInputSamples));
    m_inputQueue = inputQueue;
    QSharedPointer<FixedDataQueue<double> > dcmQueue(new FixedDataQueue<double>(noOutputSamples()));
    m_dsmQueue = dcmQueue;
    QSharedPointer<FixedDataQueue<uint8_t> > outputQueue(new FixedDataQueue<uint8_t>(noOutputBytes()));
    m_outputQueue = outputQueue;

    if(!startThreads())
        return false;

    return true;
}

//-------------------------------------------------------------------------------------------

int PCMToDSD::pullInterleaved(sample_t *out, int noPCMSamples)
{
    if(out == NULL || noPCMSamples <= 0)
        return 0;

    int pos = 0;
    while(availablePCMSamples() > 0 && pos < noPCMSamples)
    {
        if(m_outputBuffer == NULL)
        {
            m_outputBuffer = m_outputQueue->pull();
            m_outputBufferAmount = 0;
            m_available -= m_outputQueue->arraySize();
        }

        if(m_dataType == e_SampleDSD8LSB || m_dataType == e_SampleDSD8MSB)
        {
            uint8_t *o = reinterpret_cast<uint8_t *>(&out[pos * m_noChannels]);
			o += sizeof(sample_t) * m_channelIndex;
            while(pos < noPCMSamples && m_outputBufferAmount < m_outputQueue->arraySize())
            {
                for(int idx = 0; idx < sizeof(sample_t); idx++)
                {
                    o[idx] = m_outputBuffer[m_outputBufferAmount];                    
                    m_outputBufferAmount++;
                }
                pos++;
                o += sizeof(sample_t) * m_noChannels;
            }
        }
        else
        {
            uint32_t *oInt = reinterpret_cast<uint32_t *>(out);
            uint32_t *o = reinterpret_cast<uint32_t *>(&oInt[pos * m_noChannels]);
            while(pos < noPCMSamples && m_outputBufferAmount < m_outputQueue->arraySize())
            {
                uint32_t s = (m_markerInc & 0x01) ? 0xfffa0000 : 0x00050000;
                uint8_t a0 = m_outputBuffer[m_outputBufferAmount + 0];
                uint8_t a1 = m_outputBuffer[m_outputBufferAmount + 1];
                s |= ((static_cast<uint32_t>(a0) << 8) & 0x0000ff00) | (static_cast<uint32_t>(a1) & 0x000000ff);
                if(m_dataType == e_SampleInt32)
                {
                    s <<= 8;
                }
                o[m_channelIndex] = s;
                o += m_noChannels;
                m_outputBufferAmount += 2;
                m_markerInc++;
                pos++;
            }
        }

        if(m_outputBufferAmount >=  m_outputQueue->arraySize())
        {
            m_outputQueue->free(m_outputBuffer);
            m_outputBuffer = NULL;
        }
    }
    return pos;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

