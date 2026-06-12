#include "engine/inc/PCMToDSDProcessor.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

PCMToDSDProcessor::PCMToDSDProcessor() : m_dsdTimes(0),
    m_delayPCMCount(0),
    m_pcmSampleOffset(0),
    m_convertors(),
    m_partInfo(),
    m_delayData(),
    m_isFinal(false)
{}

//-------------------------------------------------------------------------------------------

PCMToDSDProcessor::~PCMToDSDProcessor()
{
    done();
}

//-------------------------------------------------------------------------------------------

bool PCMToDSDProcessor::init(CodecDataType dataType, int inputFreq, int dsdTimes, int noChannels)
{
    bool res;

    done();

    res = true;
    for(int chIdx = 0; chIdx < noChannels && res; chIdx++)
    {
        QSharedPointer<PCMToDSD> pConv(new PCMToDSD());
        if(pConv->initInterleaved(inputFreq, dsdTimes, dataType, chIdx, noChannels))
        {
            m_convertors.append(pConv);
        }
        else
        {
            res = false;
        }
    }
    if(res)
    {
        m_dsdTimes = dsdTimes;
        m_delayPCMCount = delayNoSamples();
        m_pcmSampleOffset = 0;
        m_partInfo.clear();
        QSharedPointer<RData> pDelay(new RData(m_delayPCMCount, noChannels, noChannels));
        m_delayData = pDelay;
        m_isFinal = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void PCMToDSDProcessor::done()
{
    m_dsdTimes = 0;
    m_delayPCMCount = 0;
    m_pcmSampleOffset = 0;
    m_convertors.clear();
    m_partInfo.clear();
    m_delayData.clear();
    m_isFinal = true;
}

//-------------------------------------------------------------------------------------------

void PCMToDSDProcessor::push(const RData& data)
{
    for(int idx = 0; idx < data.noParts(); idx++)
    {
        const RData::Part& part = data.partConst(idx);
        for(auto pConvertor : m_convertors)
        {
            pConvertor->push(data.partDataConst(idx), part.lengthConst());
        }
        m_partInfo.append(qMakePair(part.lengthConst(), part.startConst()));
    }
}

//-------------------------------------------------------------------------------------------

int PCMToDSDProcessor::delayNoSamples() const
{
    QSharedPointer<PCMToDSD> pConvertor = m_convertors.at(0);
    int noBytes = (1024 * m_dsdTimes) >> 3;
    int noSamples;
    if(pConvertor->dataType() == e_SampleDSD8LSB || pConvertor->dataType() == e_SampleDSD8MSB)
    {
        noSamples = noBytes / 8;
    }
    else
    {
        noSamples = noBytes / 4;
    }
    return noSamples;
}

//-------------------------------------------------------------------------------------------

void PCMToDSDProcessor::finalise()
{
    if(!m_isFinal)
    {
        QSharedPointer<PCMToDSD> pConvertor = m_convertors.at(0);
        int sN = pConvertor->availablePCMSamples();
        int noSamples = delayNoSamples();
        sample_t *x = new sample_t [noSamples * m_convertors.size()];
        for(int idx = 0; idx < noSamples * m_convertors.size(); idx++)
        {
            x[idx] = 0.0;
        }

        while((sN + noSamples) < pConvertor->availablePCMSamples())
        {
            for(auto pConvertor : m_convertors)
            {
                pConvertor->push(x, noSamples);
            }
        }

        delete [] x;
        m_isFinal = true;
    }
}

//-------------------------------------------------------------------------------------------

bool PCMToDSDProcessor::isFinalised() const
{
    return m_isFinal;
}

//-------------------------------------------------------------------------------------------

int PCMToDSDProcessor::available() const
{
    int avail = 0;
    if(!m_convertors.isEmpty())
    {
        QSharedPointer<PCMToDSD> pConvertor = m_convertors.at(0);
        avail = pConvertor->availablePCMSamples();
    }
    return avail;
}

//-------------------------------------------------------------------------------------------

int PCMToDSDProcessor::noOutputPCMSamples(int noIn) const
{
    QSharedPointer<PCMToDSD> pConvertor = m_convertors.at(0);
    int noOut;
    int dsdBytesOut = (noIn * pConvertor->noOutputBytes()) / pConvertor->noInputSamples();
    if(pConvertor->dataType() == e_SampleDSD8LSB || pConvertor->dataType() == e_SampleDSD8MSB)
    {
        noOut = dsdBytesOut / 8;
    }
    else
    {
        noOut = dsdBytesOut / 4;
    }
    return noOut;
}

//-------------------------------------------------------------------------------------------

double PCMToDSDProcessor::timePerOuputPCMSample() const
{
    QSharedPointer<PCMToDSD> pConvertor = m_convertors.at(0);
    double inInc = 1.0 / static_cast<double>(pConvertor->inputFrequency());
    int dsdBytesPerSample = (pConvertor->dataType() == e_SampleDSD8LSB || pConvertor->dataType() == e_SampleDSD8MSB) ? 8 : 4;
    double ratio = static_cast<double>(pConvertor->inputFrequency()) / static_cast<double>(pConvertor->noOutputBytes() / dsdBytesPerSample);
    double outInc = inInc * ratio;
    return outInc;
}

//-------------------------------------------------------------------------------------------

bool PCMToDSDProcessor::areTwoPartsSequential(int idx) const
{
    bool res = true;
    QSharedPointer<PCMToDSD> pConvertor = m_convertors.at(0);

    if((idx + 1) < m_partInfo.size())
    {
        double eT = static_cast<double>(m_partInfo.at(idx).second);
        double incPerSample = 1.0 / static_cast<double>(pConvertor->inputFrequency());
        eT += static_cast<double>(m_partInfo.at(idx).first) * incPerSample;
        double sT = static_cast<double>(m_partInfo.at(idx + 1).second);
        double minT = eT - incPerSample;
        double maxT = eT + incPerSample;
        if(minT < sT && sT < maxT)
        {
            res = true;
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void PCMToDSDProcessor::pull(RData& data)
{
    int amount;
    double outInc;
    QSharedPointer<PCMToDSD> pConv = m_convertors.at(0);

    while(m_delayPCMCount > 0)
    {
        sample_t *x = m_delayData->data();
        for(auto pConvertor : m_convertors)
        {
            amount = (pConvertor->availablePCMSamples() < m_delayPCMCount) ? pConvertor->availablePCMSamples() : m_delayPCMCount;
            amount = pConvertor->pullInterleaved(x, amount);
        }
        m_delayPCMCount -= amount;
    }

    outInc = timePerOuputPCMSample();
    while(data.rLength() > 0 && pConv->availablePCMSamples() > 0)
    {
        int idx;
        int noSamplesCurrent = noOutputPCMSamples(m_partInfo.at(0).first);
        int noSamplesRemain = noSamplesCurrent - m_pcmSampleOffset;
        double sT = static_cast<double>(m_partInfo.at(0).second) + (static_cast<double>(m_pcmSampleOffset) * outInc);
        
        idx = 0;
        if(noSamplesRemain < data.rLength() && noSamplesRemain < pConv->availablePCMSamples())
        {
            amount = noSamplesRemain;
            m_pcmSampleOffset = 0;

            while((idx + 1) < m_partInfo.size() && amount < data.rLength() && amount < pConv->availablePCMSamples())
            {
                if(areTwoPartsSequential(idx))
                {
                    int nPOutSamples = noOutputPCMSamples(m_partInfo.at(idx + 1).first);
                    if((amount + nPOutSamples) < data.rLength())
                    {
                        amount += nPOutSamples;
                        idx++;
                    }
                    else if(data.rLength() < pConv->availablePCMSamples())
                    {
                        amount += data.rLength();
                        m_pcmSampleOffset += data.rLength();
                    }
                    else
                    {
                        amount += pConv->availablePCMSamples();
                        m_pcmSampleOffset += pConv->availablePCMSamples();
                    }
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            amount = (data.rLength() < pConv->availablePCMSamples()) ? data.rLength() : pConv->availablePCMSamples();
            m_pcmSampleOffset += amount;
        }

        engine::RData::Part& part = data.nextPart();
        sample_t *x = data.partData(data.noParts() - 1);
        for(const auto pConvertor : m_convertors)
        {
            Q_ASSERT(pConvertor->pullInterleaved(x, amount) == amount);
        }
        part.start() = sT;
        if(data.noParts() == 1)
        {
            data.start() = sT;
        }
        double eT = static_cast<double>(m_partInfo.at(idx).second) + (static_cast<double>(m_pcmSampleOffset) * outInc);
        part.end() = eT;
        data.end() = eT;
       
        while(idx > 0)
        {
            m_partInfo.removeFirst();
            idx--;
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

