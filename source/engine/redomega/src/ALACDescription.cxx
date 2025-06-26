#include "engine/redomega/inc/ALACDescription.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

ALACDescription::ALACDescription() : m_sampleRate(0.0),
    m_formatID(0),
    m_formatFlags(0),
    m_bytesPerPacket(0),
    m_framesPerPacket(0),
    m_bytesPerFrame(0),
    m_channelsPerFrame(0),
    m_bitsPerChannel(0)
{}

//-------------------------------------------------------------------------------------------

ALACDescription::~ALACDescription()
{}

//-------------------------------------------------------------------------------------------

void ALACDescription::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "ALACDescription::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool ALACDescription::read(ALACSequence *seq)
{
    m_sampleRate = seq->readDouble();
    m_formatID = static_cast<tuint32>(seq->readBitsI(32));
    m_formatFlags = seq->readBitsI(32);
    m_bytesPerPacket = seq->readBitsI(32);
    m_framesPerPacket = seq->readBitsI(32);
    m_channelsPerFrame = seq->readBitsI(32);
    m_bitsPerChannel = seq->readBitsI(32);
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
