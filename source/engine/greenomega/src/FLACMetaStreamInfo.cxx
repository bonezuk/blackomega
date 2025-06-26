#include "engine/greenomega/inc/FLACMetaStreamInfo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

FLACMetaStreamInfo::FLACMetaStreamInfo() : FLACMetaBase(e_StreamInfo),
    m_minBlockSize(0),
    m_maxBlockSize(0),
    m_minFrameSize(0),
    m_maxFrameSize(0),
    m_frequency(44100),
    m_channels(2),
    m_bitsPerSample(0),
    m_totalSamples(0),
    m_md5SignatureUpper(0),
    m_md5SignatureLower(0)
{}

//-------------------------------------------------------------------------------------------

FLACMetaStreamInfo::~FLACMetaStreamInfo()
{}

//-------------------------------------------------------------------------------------------

void FLACMetaStreamInfo::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "FLACMetaStreamInfo::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool FLACMetaStreamInfo::read(engine::Sequence *seq)
{
    tuint32 a,b;
    seq->readBitsI(24);
    m_minBlockSize = seq->readBitsI(16);
    m_maxBlockSize = seq->readBitsI(16);
    m_minFrameSize = seq->readBitsI(24);
    m_maxFrameSize = seq->readBitsI(24);
    m_frequency = seq->readBitsI(20);
    m_channels = seq->readBitsI(3) + 1;
    m_bitsPerSample = seq->readBitsI(5) + 1;
    a = seq->readBitsI(4);
    b = seq->readBitsI(32);
    m_totalSamples = ((static_cast<tuint64>(a) << 32) & 0xffffffff00000000ULL) | (static_cast<tuint64>(b) & 0x00000000ffffffffULL);
    m_md5SignatureUpper = seq->read64BitsI();
    m_md5SignatureLower = seq->read64BitsI();
    return true;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp FLACMetaStreamInfo::length() const
{
    double l = static_cast<tfloat64>(m_totalSamples) / static_cast<tfloat64>(m_frequency);
    return common::TimeStamp(l);
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
