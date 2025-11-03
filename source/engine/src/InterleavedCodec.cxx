#include "engine/inc/InterleavedCodec.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

InterleavedCodec::InterleavedCodec(Codec::CodecType type,QObject *parent) : Codec(type,parent),
    m_state(-1),
    m_outLen(0),
    m_outOffset(0),
    m_pBuffer(0),
    m_time(0),
    m_dataType(e_SampleFloat)
{}

//-------------------------------------------------------------------------------------------

InterleavedCodec::~InterleavedCodec()
{}

//-------------------------------------------------------------------------------------------

bool InterleavedCodec::init()
{
    m_initFlag = true;
    m_state = 0;
    return true;
}

//-------------------------------------------------------------------------------------------

bool InterleavedCodec::next(AData& data)
{
    tint i,len;
    sample_t *buffer;
    bool res = true;
    engine::RData& rData = dynamic_cast<engine::RData&>(data);

    if(!rData.noParts())
    {
        data.start() = m_time;
    }

    if(m_state>=0)
    {
        engine::RData::Part *part = &(rData.nextPart());

        buffer = rData.partData(rData.noParts() - 1);
        part->start() = m_time;

        i = 0;
        len = rData.rLength();

        while(i<len && res)
        {
            switch(m_state)
            {
                case 0:
                    {
                        if(decodeNextPacket(m_outLen))
                        {
                            m_pBuffer = getPacketBuffer();
                            m_state = 1;
                            m_outOffset = 0;
                        }
                        else
                        {
                            m_state = -1;
                            res = false;
                        }
                    }
                    break;

                case 1:
                    {
                        tint amount;

                        amount = len - i;
                        if(amount > (m_outLen - m_outOffset))
                        {
                            amount = m_outLen - m_outOffset;
                        }
                        if(amount > 0)
                        {
                            readDecodedData(buffer,i,amount);

                            m_outOffset += amount;
                            i += amount;
                            m_time += static_cast<tfloat64>(amount) / static_cast<tfloat64>(frequency());
                        }
                        if(m_outOffset >= m_outLen)
                        {
                            m_state = 0;
                        }
                    }
                    break;

                default:
                    res = false;
                    break;
            }
        }

        part->length() = i;
        part->end() = m_time;
        part->done() = true;
        part->setDataType(m_dataType);
        data.end() = m_time;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void InterleavedCodec::readDecodedData(sample_t *output,tint sampleOffset,tint amount)
{
    tint noChs = noChannels();
    tint aTotal = amount * noChs;
    tint offset = sampleOffset * noChs;
    tint bps = bytesPerSample();

    if(m_dataType == e_SampleInt16)
    {
        tint16 *oInt16 = reinterpret_cast<tint16 *>(output);
        for(int j=0;j<aTotal;j++)
        {
            oInt16[offset + j] = readSampleInt16(m_pBuffer);
            m_pBuffer += bps;
        }
    }
    else if(m_dataType == e_SampleInt24)
    {
        tint32 *oInt24 = reinterpret_cast<tint32 *>(output);
        for(int j=0;j<aTotal;j++)
        {
            oInt24[offset + j] = readSampleInt24(m_pBuffer);
            m_pBuffer += bps;
        }
    }
    else if(m_dataType == e_SampleInt32)
    {
        tint32 *oInt32 = reinterpret_cast<tint32 *>(output);
        for(int j=0;j<aTotal;j++)
        {
            oInt32[offset + j] = readSampleInt32(m_pBuffer);
            m_pBuffer += bps;
        }
    }
    else
    {
        for(int j=0;j<aTotal;j++)
        {
            output[offset + j] = readSample(m_pBuffer);
            m_pBuffer += bps;
        }
    }
}

//-------------------------------------------------------------------------------------------

tint16 InterleavedCodec::readSampleInt16(char *buffer)
{
    tint16 *x = reinterpret_cast<tint16 *>(buffer);
    return static_cast<tint16>(*x);
}

//-------------------------------------------------------------------------------------------

tint32 InterleavedCodec::readSampleInt24(char *buffer)
{
    tint32 *x = reinterpret_cast<tint32 *>(buffer);
    return static_cast<tint32>(*x);
}

//-------------------------------------------------------------------------------------------

tint32 InterleavedCodec::readSampleInt32(char *buffer)
{
    tint32 *x = reinterpret_cast<tint32 *>(buffer);
    return static_cast<tint32>(*x);
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
