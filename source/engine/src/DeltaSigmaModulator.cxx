#include "engine/inc/DeltaSigmaModulator.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

DeltaSigmaModulator::DeltaSigmaModulator() : m_isLSB(true),
    m_deltaIdx(DSM_DELTA_SIZE - DSM_IIR_ORDER)
{
    m_delta = new double [DSM_DELTA_SIZE];
}

//-------------------------------------------------------------------------------------------

DeltaSigmaModulator::~DeltaSigmaModulator()
{
    delete [] m_delta;
}

//-------------------------------------------------------------------------------------------

void DeltaSigmaModulator::init(bool isLSB)
{
    const double c_noiseShapeCoefficient[2][DSM_IIR_ORDER] = {
        {
            7.9959288660876267,
            -27.975578426642503,
            55.938953909594289,
            -69.91860869807897,
            55.938953909594282,
            -27.975578426642503,
            7.9959288660876267,
            -0.99999999999999978
        },
        {
            -0.54184091028035863,
            4.6211255639903257,
            -17.319327891504788,
            37.237233820988472,
            -50.213208401223746,
            43.471176498498245,
            -23.588887732076959,
            7.3337285721047616
        }
    };

    int idx;
    
    for(idx = 0; idx < DSM_IIR_ORDER; idx++)
    {
        m_NS[0][idx] = c_noiseShapeCoefficient[0][idx];
        m_NS[1][DSM_IIR_ORDER - idx - 1] = c_noiseShapeCoefficient[1][idx];
    }
    for(idx = 0; idx < DSM_IIR_ORDER; idx++)
    {
        m_NS[0][idx] = m_NS[0][idx] - m_NS[1][idx];
    }

    m_isLSB = isLSB;
    m_deltaIdx = DSM_DELTA_SIZE - DSM_IIR_ORDER;
    memset(m_delta, 0, DSM_DELTA_SIZE * sizeof(double));
}

//-------------------------------------------------------------------------------------------

void DeltaSigmaModulator::process(const double *pcm, uint8_t *dsd, int len)
{
    const uint8_t maskMSB[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
    const uint8_t maskLSB[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
    const uint8_t *mask = (m_isLSB) ? maskLSB : maskMSB;
    const double c_gain = 0.5;

    int idx, t;
    double xI, eY, d;

    memset(dsd, 0, len >> 3);

    for(idx = 0; idx < len; idx++)
    {
        xI = pcm[idx] * c_gain;

        for(t = 0; t < DSM_IIR_ORDER; t++)
        {
            xI += m_NS[0][t] * m_delta[m_deltaIdx + t];
        }

        if(xI >= 0.0)
        {
            dsd[idx >> 3] |= mask[idx & 0x7];
            eY = -1.0;
        }
        else
        {
            eY = 1.0;
        }

        d = xI + eY;
        for(t = 0; t < DSM_IIR_ORDER; t++)
        {
            d += m_NS[1][t] * m_delta[m_deltaIdx + t];
        }

        m_deltaIdx--;
        if(m_deltaIdx < 0)
        {
            m_deltaIdx = DSM_DELTA_SIZE - DSM_IIR_ORDER;
            for(t = 1; t < DSM_IIR_ORDER; t++)
            {
                m_delta[m_deltaIdx + t] = m_delta[t - 1];
            }
        }
        m_delta[m_deltaIdx] = d;
    }
}

//-------------------------------------------------------------------------------------------

bool  DeltaSigmaModulator::isLSB() const
{
    return m_isLSB;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
