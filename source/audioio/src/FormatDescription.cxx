#include "audioio/inc/FormatDescription.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

FormatDescription::FormatDescription() : m_dataType(FormatDescription::e_DataSignedInteger),
    m_bits(16),
    m_channels(2),
    m_frequency(44100),
    m_isLittleEndian(true)
{}

//-------------------------------------------------------------------------------------------

FormatDescription::FormatDescription(DataType type,tint noBits,tint noChannels,tint freq) : m_dataType(FormatDescription::e_DataSignedInteger),
    m_bits(16),
    m_channels(2),
    m_frequency(44100),
    m_isLittleEndian(true)
{
    setTypeOfData(type);
    setNumberOfBits(noBits);
    setNumberOfChannels(noChannels);
    setFrequency(freq);
}

//-------------------------------------------------------------------------------------------

FormatDescription::FormatDescription(DataType type,tint noBits,tint noChannels,tint freq,bool littleEndian) : m_dataType(FormatDescription::e_DataSignedInteger),
    m_bits(16),
    m_channels(2),
    m_frequency(44100),
    m_isLittleEndian(littleEndian)
{
    setTypeOfData(type);
    setNumberOfBits(noBits);
    setNumberOfChannels(noChannels);
    setFrequency(freq);
}

//-------------------------------------------------------------------------------------------

FormatDescription::FormatDescription(const FormatDescription& rhs)  : m_dataType(FormatDescription::e_DataSignedInteger),
    m_bits(16),
    m_channels(2),
    m_frequency(44100)
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

const FormatDescription& FormatDescription::operator = (const FormatDescription& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void FormatDescription::copy(const FormatDescription& rhs)
{
    m_dataType = rhs.m_dataType;
    m_bits = rhs.m_bits;
    m_channels = rhs.m_channels;
    m_frequency = rhs.m_frequency;
    m_isLittleEndian = rhs.m_isLittleEndian;
}

//-------------------------------------------------------------------------------------------

bool FormatDescription::isEqual(const FormatDescription& rhs) const
{
    return (m_dataType==rhs.m_dataType && m_bits==rhs.m_bits && m_channels==rhs.m_channels && m_frequency==rhs.m_frequency && m_isLittleEndian==rhs.m_isLittleEndian);
}

//-------------------------------------------------------------------------------------------

bool operator == (const FormatDescription& a,const FormatDescription& b)
{
    return a.isEqual(b);
}

//-------------------------------------------------------------------------------------------

bool operator != (const FormatDescription& a,const FormatDescription& b)
{
    return !(a.isEqual(b));
}

//-------------------------------------------------------------------------------------------

const FormatDescription::DataType& FormatDescription::typeOfData() const
{
    return m_dataType;
}

//-------------------------------------------------------------------------------------------

bool FormatDescription::setTypeOfData(DataType type)
{
    bool res = true;

    switch(type)
    {
        case e_DataFloatDouble:
            m_dataType = type;
            m_bits = 64;
            break;

        case e_DataFloatSingle:
            m_dataType = type;
            m_bits = 32;
            break;

        case e_DataSignedInteger:
        case e_DataUnsignedInteger:
            m_dataType = type;
            break;

        default:
            res = false;
            break;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

const tint& FormatDescription::bits() const
{
    return m_bits;
}

//-------------------------------------------------------------------------------------------

bool FormatDescription::setNumberOfBits(tint noBits)
{
    bool res;

    if(noBits>=1 && noBits<=32)
    {
        m_bits = noBits;
        res = true;
    }
    else if(noBits==64)
    {
        m_dataType = e_DataFloatDouble;
        m_bits = 64;
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint FormatDescription::bitsIndex() const
{
    tint idx = -1;

    switch(m_bits)
    {
        case 8:
            idx = 0;
            break;

        case 16:
            idx = 1;
            break;

        case 24:
            idx = 2;
            break;

        case 32:
            idx = (m_dataType==e_DataFloatSingle) ? 4 : 3;
            break;

        case 64:
            idx = 5;
            break;

        default:
            if(m_bits>=1 && m_bits<=7)
            {
                idx = m_bits + 5;
            }
            else if(m_bits>=9 && m_bits<=15)
            {
                idx = m_bits + 4;
            }
            else if(m_bits>=17 && m_bits<=23)
            {
                idx = m_bits + 3;
            }
            else if(m_bits>=25 && m_bits<=31)
            {
                idx = m_bits + 2;
            }
            break;
    }
    return idx;
}

//-------------------------------------------------------------------------------------------

bool FormatDescription::setBitsIndex(tint idx)
{
    bool res = true;

    if(idx==4)
    {
        m_dataType = e_DataFloatSingle;
        m_bits = 32;
    }
    else if(idx==5)
    {
        m_dataType = e_DataFloatDouble;
        m_bits = 64;
    }
    else if(idx>=0 && idx<=33)
    {
        if(idx==0)
        {
            m_bits = 8;
        }
        else if(idx==1)
        {
            m_bits = 16;
        }
        else if(idx==2)
        {
            m_bits = 24;
        }
        else if(idx==3)
        {
            m_bits = 32;
        }
        else if(idx>=6 && idx<=12)
        {
            m_bits = idx - 5;
        }
        else if(idx>=13 && idx<=19)
        {
            m_bits = idx - 4;
        }
        else if(idx>=20 && idx<=26)
        {
            m_bits = idx - 3;
        }
        else
        {
            m_bits = idx - 2;
        }

        if(m_dataType==e_DataFloatSingle || m_dataType==e_DataFloatDouble)
        {
            m_dataType = e_DataSignedInteger;
        }
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

const tint& FormatDescription::channels() const
{
    return m_channels;
}

//-------------------------------------------------------------------------------------------

bool FormatDescription::setNumberOfChannels(tint noChannels)
{
    bool res;

    if(noChannels>=1 && noChannels<=8)
    {
        m_channels = noChannels;
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint FormatDescription::channelsIndex() const
{
    return m_channels - 1;
}

//-------------------------------------------------------------------------------------------

bool FormatDescription::setChannelsIndex(tint idx)
{
    bool res;

    if(idx>=0 && idx<8)
    {
        m_channels = idx + 1;
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

const tint& FormatDescription::frequency() const
{
    return m_frequency;
}

//-------------------------------------------------------------------------------------------

bool FormatDescription::setFrequency(tint freq)
{
    bool res;

    switch(freq)
    {
        case 8000:
        case 11025:
        case 12000:
        case 16000:
        case 22050:
        case 24000:
        case 32000:
        case 44100:
        case 48000:
        case 64000:
        case 88200:
        case 96000:
        case 176400:
        case 192000:
        case 352800:
        case 384000:
        case 705600:
        case 768000:
            m_frequency = freq;
            res = true;
            break;

        default:
            res = false;
            break;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint FormatDescription::frequencyIndex() const
{
    int index;

    switch(m_frequency)
    {
        case 8000:
            index = 0;
            break;
        case 11025:
            index = 1;
            break;
        case 12000:
            index = 2;
            break;
        case 16000:
            index = 3;
            break;
        case 22050:
            index = 4;
            break;
        case 24000:
            index = 5;
            break;
        case 32000:
            index = 6;
            break;
        case 44100:
            index = 7;
            break;
        case 48000:
            index = 8;
            break;
        case 64000:
            index = 9;
            break;
        case 88200:
            index = 10;
            break;
        case 96000:
            index = 11;
            break;
        case 176400:
            index = 12;
            break;
        case 192000:
            index = 13;
            break;
        case 352800:
            index = 14;
            break;
        case 384000:
            index = 15;
            break;
        case 705600:
            index = 16;
            break;
        case 768000:
            index = 17;
            break;
        default:
            index = -1;
            break;
    }
    return index;
}

//-------------------------------------------------------------------------------------------

bool FormatDescription::setFrequencyIndex(tint idx)
{
    bool res = true;

    switch(idx)
    {
        case 0:
            m_frequency = 8000;
            break;

        case 1:
            m_frequency = 11025;
            break;

        case 2:
            m_frequency = 12000;
            break;

        case 3:
            m_frequency = 16000;
            break;

        case 4:
            m_frequency = 22050;
            break;

        case 5:
            m_frequency = 24000;
            break;

        case 6:
            m_frequency = 32000;
            break;

        case 7:
            m_frequency = 44100;
            break;

        case 8:
            m_frequency = 48000;
            break;

        case 9:
            m_frequency = 64000;
            break;

        case 10:
            m_frequency = 88200;
            break;

        case 11:
            m_frequency = 96000;
            break;

        case 12:
            m_frequency = 176400;
            break;

        case 13:
            m_frequency = 192000;
            break;

        case 14:
            m_frequency = 352800;
            break;

        case 15:
            m_frequency = 384000;
            break;

        case 16:
            m_frequency = 705600;
            break;

        case 17:
            m_frequency = 768000;
            break;

        default:
            res = false;
            break;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool FormatDescription::isLittleEndian() const
{
    return m_isLittleEndian;
}

//-------------------------------------------------------------------------------------------

bool FormatDescription::isBigEndian() const
{
    return !m_isLittleEndian;
}

//-------------------------------------------------------------------------------------------

void FormatDescription::setEndian(bool littleEndian)
{
    m_isLittleEndian = littleEndian;
}

//-------------------------------------------------------------------------------------------

QSet<tint> FormatDescription::setOfFrequencies()
{
    QSet<tint> frequencies;
    frequencies.insert(8000);
    frequencies.insert(11025);
    frequencies.insert(12000);
    frequencies.insert(16000);
    frequencies.insert(22050);
    frequencies.insert(24000);
    frequencies.insert(32000);
    frequencies.insert(44100);
    frequencies.insert(48000);
    frequencies.insert(64000);
    frequencies.insert(88200);
    frequencies.insert(96000);
    frequencies.insert(176400);
    frequencies.insert(192000);
    frequencies.insert(352800);
    frequencies.insert(384000);
    frequencies.insert(705600);
    frequencies.insert(768000);
    return frequencies;
}

//-------------------------------------------------------------------------------------------

QString FormatDescription::description() const
{
    QString desc;

    desc = "dt=";
    switch(m_dataType)
    {
        case e_DataFloatSingle: desc += "FL32"; break;
        case e_DataFloatDouble: desc += "FL64"; break;
        case e_DataSignedInteger: desc += "SINT"; break;
        case e_DataUnsignedInteger: desc += "UINT"; break;
        default: desc += "UNKNOWN"; break;
    }
    desc += ", ch=" + QString::number(m_channels) + ", bits=" + QString::number(m_bits);
    desc += ", freq=" + QString::number(m_frequency) + ", order=";
    desc += (m_isLittleEndian) ? "LE" : "BE";
    return desc;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
