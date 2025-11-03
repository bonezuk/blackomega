#include "engine/whiteomega/inc/ProgramConfig.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

ProgramConfig::ProgramConfig() : m_elementInstanceTag(0),
    m_objectType(0),
    m_samplingFrequencyIndex(0),
    m_numFrontChannelElements(0),
    m_numSideChannelElements(0),
    m_numBackChannelElements(0),
    m_numLfeChannelElements(0),
    m_numAssocDataElements(0),
    m_numValidCcElements(0),
    m_monoMixdownPresent(false),
    m_monoMixdownElementNumber(0),
    m_stereoMixdownPresent(false),
    m_stereoMixdownElementNumber(0),
    m_matrixMixdownIdxPresent(false),
    m_matrixMixdownIdx(0),
    m_pseudoSurroundEnable(0),
    m_comment()
{
    tint i;

    for(i=0;i<16;++i)
    {
        m_frontElementIsCpe[i] = false;
        m_sideElementIsCpe[i] = false;
        m_backElementIsCpe[i] = false;
        m_ccElementIsIndSw[i] = false;
    }
    ::memset(m_frontElementTagSelect,0,16 * sizeof(tint));
    ::memset(m_sideElementTagSelect,0,16 * sizeof(tint));
    ::memset(m_backElementTagSelect,0,16 * sizeof(tint));
    ::memset(m_lfeElementTagSelect,0,4 * sizeof(tint));
    ::memset(m_assocDataElementTagSelect,0,8 * sizeof(tint));
    ::memset(m_validCcElementTagSelect,0,16 * sizeof(tint));
}

//-------------------------------------------------------------------------------------------

ProgramConfig::~ProgramConfig()
{}

//-------------------------------------------------------------------------------------------

bool ProgramConfig::read(Sequence *seq)
{
    tint i;

    m_elementInstanceTag = seq->readBitsI(4);
    m_objectType = seq->readBitsI(2);
    m_samplingFrequencyIndex = seq->readBitsI(4);
    m_numFrontChannelElements = seq->readBitsI(4);
    m_numSideChannelElements = seq->readBitsI(4);
    m_numBackChannelElements = seq->readBitsI(4);
    m_numLfeChannelElements = seq->readBitsI(2);
    m_numAssocDataElements = seq->readBitsI(3);
    m_numValidCcElements = seq->readBitsI(4);
    m_monoMixdownPresent = (seq->readBitI()) ? true : false;
    if(m_monoMixdownPresent)
    {
        m_monoMixdownElementNumber = seq->readBitsI(4);
    }
    m_stereoMixdownPresent = (seq->readBitI()) ? true : false;
    if(m_stereoMixdownPresent)
    {
        m_stereoMixdownElementNumber = seq->readBitsI(4);
    }
    m_matrixMixdownIdxPresent = (seq->readBitI()) ? true : false;
    if(m_matrixMixdownIdxPresent)
    {
        m_matrixMixdownIdx = seq->readBitsI(2);
        m_pseudoSurroundEnable = (seq->readBitI()) ? true : false;
    }
    for(i=0;i<m_numFrontChannelElements;++i)
    {
        m_frontElementIsCpe[i] = (seq->readBitI()) ? true : false;
        m_frontElementTagSelect[i] = seq->readBitsI(4);
    }
    for(i=0;i<m_numSideChannelElements;++i)
    {
        m_sideElementIsCpe[i] = (seq->readBitI()) ? true : false;
        m_sideElementTagSelect[i] = seq->readBitsI(4);
    }
    for(i=0;i<m_numBackChannelElements;++i)
    {
        m_backElementIsCpe[i] = (seq->readBitI()) ? true : false;
        m_backElementTagSelect[i] = seq->readBitsI(4);
    }
    for(i=0;i<m_numLfeChannelElements;++i)
    {
        m_lfeElementTagSelect[i] = seq->readBitsI(4);
    }
    for(i=0;i<m_numAssocDataElements;++i)
    {
        m_assocDataElementTagSelect[i] = seq->readBitsI(4);
    }
    for(i=0;i<m_numValidCcElements;++i)
    {
        m_ccElementIsIndSw[i] = (seq->readBitI()) ? true : false;
        m_validCcElementTagSelect[i] = seq->readBitsI(4);
    }

    {
        tint len;
        tubyte *tmp;

        seq->byteAlignment();
        len = seq->readBitsI(8);
        if(len > 0)
        {
            tmp = new tubyte [len + 1];
            for(i=0;i<len;++i)
            {
                tmp[i] = static_cast<tubyte>(seq->readBitsI(8));
            }
            tmp[len] = 0x00;
            m_comment = QString::fromUtf8(reinterpret_cast<const tchar *>(tmp));
            delete [] tmp;
        }
        else
        {
            m_comment = "";
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
