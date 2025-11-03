#include "engine/toneomega/inc/ToneCodec.h"
#include "engine/inc/FormatType.h"
#include "engine/inc/FormatTypeFromFloat.h"

//-------------------------------------------------------------------------------------------
/*
struct ToneGenerator
{
    unsigned int phase;
    unsigned int phaseInc;
};

#define TONE_BIT_LENGTH 12
#define TONE_BIT_FRACTION (32 - TONE_BIT_LENGTH)
#define TONE_ARRAY_LENGTH (1 << TONE_BIT_LENGTH)

#define TONE_BIT_MASK 0x00000fff
#define TONE_BIT_FRACTION_MASK 0x000fffff

int g_phaseArray[TONE_ARRAY_LENGTH];

//-------------------------------------------------------------------------------------------

tint sampleTo32BitsInteger(float v)
{
    int x;
    float n;

    if(v >= 0.0f)
    {
        const float d = 2147483647.0f;
        n = (v + (1.0f / (d * 2.0f))) * d;
        x = (int)floorf(n);
        if(x < 0)
        {
            x = 0x7fffffff;
        }
    }
    else
    {
        const float d = 2147483648.0f;
        n = 0.0f - (((-v) + (1.0f / (d * 2.0f))) * d);
        x = (int)ceilf(n);
        if(x > 0)
        {
            x = 0x80000000;
        }
    }
    return x;
}

void initTone(struct ToneGenerator *pTone,int toneFrequency,int playbackFrequency)
{
    int i;
    float phaseInc;

    phaseInc = (float)toneFrequency / (float)playbackFrequency;

    pTone->phase = 0;
    pTone->phaseInc = (unsigned int)(phaseInc * 4294967296.0f);

    for(i=0;i<TONE_ARRAY_LENGTH;i++)
    {
        float phase = (2.0f * c_PI_F * (float)i) / (float)(TONE_ARRAY_LENGTH);
        float v = sinf(phase);
        g_phaseArray[i] = sampleTo32BitsInteger(v);
    }
}

short interpolateValueOfSample(int p1,int p2,unsigned int phase)
{
    int v;
    unsigned int t;

    t = (phase & TONE_BIT_FRACTION_MASK) >> (TONE_BIT_FRACTION - 8);
    v = (p1 >> TONE_BIT_FRACTION - 16) + (t * ((p2 >> TONE_BIT_FRACTION - 8) - (p1 >> TONE_BIT_FRACTION - 8)));
    return (short)(v >> 16);
}

// The tone generation is done for stereo output
void generateTone(struct ToneGenerator *pTone,short *samples,int noSamples)
{
    int i;

    for(i=0;i<noSamples;i++)
    {
        int idx1,idx2;
        short v;

        idx1 = (pTone->phase >> TONE_BIT_FRACTION) & TONE_BIT_MASK;
        idx2 = idx1 + 1;
        if(idx2 >= TONE_ARRAY_LENGTH)
        {
            idx2 = 0;
        }
        v = interpolateValueOfSample(g_phaseArray[idx1],g_phaseArray[idx2],pTone->phase);

        *samples++ = v;
        *samples++ = v;

        pTone->phase += pTone->phaseInc;
    }
}
*/

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace toneomega
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(CodecFactory,Codec, \
                            ToneCodecFactory,ToneCodec, \
                            "tone",false)

//-------------------------------------------------------------------------------------------

ToneCodec::ToneCodec(QObject *parent) : Codec(e_codecTone,parent),
    m_toneType(ToneCodec::e_toneUnknown),
    m_toneFrequency(1000),
    m_playbackFrequency(44100),
    m_frameNumber(0),
    m_length(600.0),
    m_completeFlag(false),
    m_phase(0.0),
    m_phaseInc(0.0),
    m_fixedSample(0.0),
    m_intPhase(0),
    m_intPhaseInc(0),
    m_channelSelect(0)
{}

//-------------------------------------------------------------------------------------------

ToneCodec::~ToneCodec()
{}

//-------------------------------------------------------------------------------------------

void ToneCodec::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "ToneCodec::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool ToneCodec::openFrequency(const QString& name)
{
    ToneType toneType = e_toneUnknown;
    tint toneFrequency = 1000;
    tint playbackFrequency = 44100;
    bool res = false;

    if(name.length()>=3)
    {
        QString tName = name.left(3).toLower();

        if(tName=="sin")
        {
            toneType = e_toneSine;
        }
        else if(tName=="cos")
        {
            toneType = e_toneCosine;
        }

        if(toneType==e_toneSine || toneType==e_toneCosine)
        {
            tint state = 0,digitStart = 0;
            QString nName = name.mid(3).trimmed();

            for(tint i=0;i<nName.length();i++)
            {
                switch(state)
                {
                    case 0:
                    case 2:
                        if(nName.at(i).isDigit())
                        {
                            digitStart = i;
                            state++;
                        }
                        else if(nName.at(i).toLower()==QChar('l'))
                        {
                            m_channelSelect = 1;
                        }
                        else if(nName.at(i).toLower()==QChar('r'))
                        {
                            m_channelSelect = 2;
                        }
                        break;

                    case 1:
                    case 3:
                        if(!nName.at(i).isDigit())
                        {
                            bool isKHz = ((i + 3)<=nName.length() && nName.mid(i,3).toLower()=="khz");
                            tint freq = nName.mid(digitStart,i-digitStart).toInt();
                            if(isKHz)
                            {
                                freq *= 1000;
                            }
                            if(state==1)
                            {
                                toneFrequency = freq;
                            }
                            else
                            {
                                playbackFrequency = freq;
                            }
                            state++;
                        }
                        break;

                    default:
                        break;
                }
            }

            if(toneFrequency <= playbackFrequency / 2)
            {
                m_toneType = toneType;
                m_toneFrequency = toneFrequency;
                m_playbackFrequency = playbackFrequency;
                res = true;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool ToneCodec::openFixed(const QString& name)
{
    bool res = false;

    if(name.length()>=5 && name.left(5).toLower()=="fixed")
    {
        tint i;
        tint noBits = 16;
        tint state = 0,digitStart = 0;
        tint32 fixedValue = 0;
        QString tName = name.mid(5).toLower();

        for(i=0;i<tName.length() && state<4;i++)
        {
            switch(state)
            {
                case 0:
                    if(tName.at(i).isDigit())
                    {
                        digitStart = i;
                        state = 1;
                    }
                    break;

                case 1:
                    if(!tName.at(i).isDigit())
                    {
                        noBits = tName.mid(digitStart,i - digitStart).toInt();
                        state = 2;
                    }
                    break;

                case 2:
                    if(tName.at(i).toLower()==QChar('x'))
                    {
                        digitStart = i + 1;
                        state = 3;
                    }
                    break;

                case 3:
                    if(tName.at(i)==QChar('.') || tName.at(i)==QChar('_'))
                    {
                        bool ok = false;
                        tuint32 fValue = static_cast<tuint32>(tName.mid(digitStart,i - digitStart).toULongLong(&ok,16));
                        fixedValue = static_cast<tint32>(fValue);
                        state = (tName.at(i)==QChar('_')) ? 5 : 4;
                    }
                    break;

                default:
                    break;
            }
        }

        if(state==4 || state==5)
        {
            m_toneType = e_toneFixed;
            m_fixedSample = engine::toSample64FromBits(fixedValue,noBits);
            res = true;

            switch(noBits)
            {
                case 8:
                    fprintf(stdout,"Fixed 8-bits: 0x%08x\n",engine::sampleTo8BitsInteger(m_fixedSample));
                    break;

                case 16:
                    fprintf(stdout,"Fixed 16-bits: 0x%08x\n",engine::sampleTo16BitsInteger(m_fixedSample));
                    break;

                case 24:
                    fprintf(stdout,"Fixed 24-bits: 0x%08x\n",engine::sampleTo24BitsInteger(m_fixedSample));
                    break;

                case 32:
                    fprintf(stdout,"Fixed 32-bits: 0x%08x\n",engine::sampleTo32BitsInteger(m_fixedSample));
                    break;

                default:
                    res = false;
                    break;
            }

            if(state==4)
            {
                m_channelSelect = 0;
            }
            else
            {
                m_channelSelect = (tName.at(i).toLower()==QChar('l')) ? 1 : 2;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// sin_1000.tone - Sine wave at 1000Hz with playback frequency of 44100Hz
// sin_1kHz.tone - Same as above
// sin_1000_l.tone - Sine wave at 1000Hz with playback frequency of 44100Hz - left channel
// sin_1kHz_r.tone - Same as above - right channel
// cos_1000.tone - Cosine wave at 1000 Hz with playback frequency of 44100Hz
// cos_1kHz.tone - Same as above
// sin_250_192kHz - Sine wave at 250Hz with playback frequency of 192000Hz
// fixed_16bits_0x1234.tone - Generate a fixed value of 0x1234 for 16-bit LE output
// fixed_24bits_0x123456.tone - Generate a fixed value of 0x123456 for 24-bit 3-byte LE output
// fixed_32bits_0xfedc3456.tone - Generate a fixed value of 0xfedc3456 for 32-bit output
// fixed_16bits_0x1234_l.tone - Generate a fixed value of 0x1234 for 16-bit LE output - left channel
//-------------------------------------------------------------------------------------------

bool ToneCodec::open(const QString& name)
{
    bool res = false;

    if(openFrequency(name))
    {
        res = true;
    }
    else if(openFixed(name))
    {
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void ToneCodec::close()
{}

//-------------------------------------------------------------------------------------------

bool ToneCodec::init()
{
    m_initFlag = true;
    m_completeFlag = false;
    m_phase = 0.0;
    m_phaseInc = (2.0 * c_PI_D * static_cast<tfloat64>(m_toneFrequency)) / static_cast<tfloat64>(m_playbackFrequency);
    m_frameNumber = 0;
    return true;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp ToneCodec::timeFramePosition(tint64 cFrame) const
{
    common::TimeStamp cS(static_cast<tfloat64>(cFrame) / static_cast<tfloat64>(m_playbackFrequency));
    return cS;
}

//-------------------------------------------------------------------------------------------

bool ToneCodec::next(AData& data)
{
    bool res;
    RData& rData = dynamic_cast<RData&>(data);
    tint noSamples = rData.rLength();
    common::TimeStamp currentT = timeFramePosition(m_frameNumber);

    if(m_completeFlag)
    {
        return false;
    }

    if(timeFramePosition(m_frameNumber + noSamples) >= m_length)
    {
        common::TimeStamp remainingT = m_length - currentT;
        noSamples = static_cast<tint>(::floor(static_cast<tfloat64>(remainingT) * static_cast<tfloat64>(m_playbackFrequency)));
        m_completeFlag = true;
        res = false;
    }
    else
    {
        res = true;
    }

    if(noSamples > 0)
    {
        RData::Part& part = rData.nextPart();
        sample_t *x = rData.partData(rData.noParts() - 1);

        part.start() = timeFramePosition(m_frameNumber);
        if(rData.noParts()==1)
        {
            data.start() = part.start();
        }

        if(m_toneType==e_toneSine)
        {
            switch(m_channelSelect)
            {
                case 0:
                    for(tint i=0;i<noSamples;i++,m_phase+=m_phaseInc)
                    {
                        tfloat64 v;
                        if(m_phase >(2.0 * c_PI_D))
                        {
                            m_phase = fmod(m_phase,2.0 * c_PI_D);
                        }
                        v = sin(m_phase);
                        *x++ = static_cast<sample_t>(v);
                        *x++ = static_cast<sample_t>(v);
                    }
                    break;

                case 1:
                    for(tint i=0;i<noSamples;i++,m_phase+=m_phaseInc)
                    {
                        tfloat64 v;
                        if(m_phase >(2.0 * c_PI_D))
                        {
                            m_phase = fmod(m_phase,2.0 * c_PI_D);
                        }
                        v = sin(m_phase);
                        *x++ = static_cast<sample_t>(v);
                        *x++ = c_zeroSample;
                    }
                    break;

                case 2:
                    for(tint i=0;i<noSamples;i++,m_phase+=m_phaseInc)
                    {
                        tfloat64 v;
                        if(m_phase >(2.0 * c_PI_D))
                        {
                            m_phase = fmod(m_phase,2.0 * c_PI_D);
                        }
                        v = sin(m_phase);
                        *x++ = c_zeroSample;
                        *x++ = static_cast<sample_t>(v);
                    }
                    break;
            }
        }
        else if(m_toneType==e_toneCosine)
        {
            switch(m_channelSelect)
            {
                case 0:
                    for(tint i=0;i<noSamples;i++,m_phase+=m_phaseInc)
                    {
                        tfloat64 v;
                        if(m_phase >(2.0 * c_PI_D))
                        {
                            m_phase = fmod(m_phase,2.0 * c_PI_D);
                        }
                        v = cos(m_phase);
                        *x++ = static_cast<sample_t>(v);
                        *x++ = static_cast<sample_t>(v);
                    }
                    break;

                case 1:
                    for(tint i=0;i<noSamples;i++,m_phase+=m_phaseInc)
                    {
                        tfloat64 v;
                        if(m_phase >(2.0 * c_PI_D))
                        {
                            m_phase = fmod(m_phase,2.0 * c_PI_D);
                        }
                        v = cos(m_phase);
                        *x++ = static_cast<sample_t>(v);
                        *x++ = c_zeroSample;
                    }
                    break;

                case 2:
                    for(tint i=0;i<noSamples;i++,m_phase+=m_phaseInc)
                    {
                        tfloat64 v;
                        if(m_phase >(2.0 * c_PI_D))
                        {
                            m_phase = fmod(m_phase,2.0 * c_PI_D);
                        }
                        v = cos(m_phase);
                        *x++ = c_zeroSample;
                        *x++ = static_cast<sample_t>(v);
                    }
                    break;
            }
        }
        else if(m_toneType==e_toneFixed)
        {
            switch(m_channelSelect)
            {
                case 0:
                    for(tint i=0;i<noSamples;i++)
                    {
                        *x++ = static_cast<sample_t>(m_fixedSample);
                        *x++ = static_cast<sample_t>(m_fixedSample);
                    }
                    break;

                case 1:
                    for(tint i=0;i<noSamples;i++)
                    {
                        *x++ = static_cast<sample_t>(m_fixedSample);
                        *x++ = c_zeroSample;
                    }
                    break;

                case 2:
                    for(tint i=0;i<noSamples;i++)
                    {
                        *x++ = c_zeroSample;
                        *x++ = static_cast<sample_t>(m_fixedSample);
                    }
                    break;
            }
        }
        else
        {
            m_completeFlag = true;
            res = false;
        }

        m_frameNumber += noSamples;
        part.length() = noSamples;
        part.end() = timeFramePosition(m_frameNumber);
        part.done() = true;
        data.end() = part.end();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool ToneCodec::isSeek() const
{
    return true;
}

//-------------------------------------------------------------------------------------------

bool ToneCodec::seek(const common::TimeStamp& v)
{
    bool res;

    if(v>=0 && v<=m_length)
    {
        m_frameNumber = static_cast<tint>(floor(static_cast<tfloat64>(v) * static_cast<tfloat64>(m_playbackFrequency)));
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool ToneCodec::isComplete() const
{
    return m_completeFlag;
}

//-------------------------------------------------------------------------------------------

bool ToneCodec::isRemote() const
{
    return false;
}

//-------------------------------------------------------------------------------------------

bool ToneCodec::isBuffered(tfloat32&)
{
    return false;
}

//-------------------------------------------------------------------------------------------

tint ToneCodec::bitrate() const
{
    return frequency() * noChannels() * 16;
}

//-------------------------------------------------------------------------------------------

tint ToneCodec::frequency() const
{
    return m_playbackFrequency;
}

//-------------------------------------------------------------------------------------------

tint ToneCodec::noChannels() const
{
    return 2;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp ToneCodec::length() const
{
    return m_length;
}

//-------------------------------------------------------------------------------------------
} // namespace toneomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
