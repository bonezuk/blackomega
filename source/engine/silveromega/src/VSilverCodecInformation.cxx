#include "engine/silveromega/inc/VSilverCodecInformation.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

VSilverCodecInformation::VSilverCodecInformation() : VSilverInfoObject(),
    m_version(0),
    m_audioChannels(0),
    m_audioSampleRate(0),
    m_bitrateMaximum(0),
    m_bitrateNominal(0),
    m_bitrateMinimum(0),
    m_shiftBlockSize_0(0),
    m_shiftBlockSize_1(0),
    m_blockSize_0(0),
    m_blockSize_1(0)
{}

//-------------------------------------------------------------------------------------------

VSilverCodecInformation::~VSilverCodecInformation()
{}

//-------------------------------------------------------------------------------------------

void VSilverCodecInformation::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VSilverCodecInformation::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VSilverCodecInformation::isValid() const
{
    bool res = true;

    if(m_version)
    {
        printError("isValid","Unsupported codec version");
        res = false;
    }
    if(m_audioChannels < 1)
    {
        printError("isValid","Invalid number of audio channels is required");
        res = false;
    }
    if(m_audioSampleRate < 1)
    {
        printError("isValid","Invalid audio smple rate");
        res = false;
    }
    if(m_blockSize_0<0 || m_blockSize_1<0 || m_blockSize_0>m_blockSize_1)
    {
        printError("isValid","Invalid block size");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverCodecInformation::read(engine::Sequence *seq)
{
    if(seq==0)
    {
        printError("read","No sequence instance given");
        return false;
    }

    skipPacketIdentifer(seq);

    m_version = seq->readBits(32);

    m_audioChannels = seq->readBits(8);
    m_audioSampleRate = seq->readBits(32);

    m_bitrateMaximum = seq->readBits(32);
    m_bitrateNominal = seq->readBits(32);
    m_bitrateMinimum = seq->readBits(32);

    m_shiftBlockSize_0 = static_cast<tuint>(seq->readBits(4));
    m_shiftBlockSize_1 = static_cast<tuint>(seq->readBits(4));

    m_blockSize_0 = 1 << m_shiftBlockSize_0;
    m_blockSize_1 = 1 << m_shiftBlockSize_1;

    if(!completePacket(seq))
    {
        printError("read","Information packet not properly terminated");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
