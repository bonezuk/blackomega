#include "engine/redomega/inc/ALACSpecificConfig.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

ALACSpecificConfig::ALACSpecificConfig() : m_frameLength(0),
    m_compatibleVersion(0),
    m_bitDepth(0),
    m_pb(0),
    m_mb(0),
    m_kb(0),
    m_numChannels(0),
    m_maxRun(0),
    m_maxFrameBytes(0),
    m_avgBitRate(0),
    m_sampleRate(0)
{}

//-------------------------------------------------------------------------------------------

ALACSpecificConfig::~ALACSpecificConfig()
{}

//-------------------------------------------------------------------------------------------

void ALACSpecificConfig::printError(const tchar *strR,const tchar *strE) const        
{
    common::Log::g_Log << "ALACSpecificConfig::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool ALACSpecificConfig::read(ALACSequence *seq)
{
    bool res = true;

    if(seq!=0)
    {
        m_frameLength = seq->readBitsI(32);
        m_compatibleVersion = seq->readBitsI(8);
        m_bitDepth = seq->readBitsI(8);
        m_pb = seq->readBitsI(8);
        m_mb = seq->readBitsI(8);
        m_kb = seq->readBitsI(8);
        m_numChannels = seq->readBitsI(8);
        m_maxRun = seq->readBitsI(16);
        m_maxFrameBytes = seq->readBitsI(32);
        m_avgBitRate = seq->readBitsI(32);
        m_sampleRate = seq->readBitsI(32);
        res = true;
    }
    else
    {
        printError("read","No sequence given to read");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
