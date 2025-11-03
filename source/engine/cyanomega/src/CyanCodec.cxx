#include "engine/cyanomega/inc/CyanCodec.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace cyanomega
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(CodecFactory,Codec, \
                            CyanCodecFactory,CyanCodec, \
                            "mpc",false)

//-------------------------------------------------------------------------------------------

CyanCodec::CyanCodec(QObject *parent) : InterleavedCodec(e_codecMPC,parent),
    m_pDecoder(),
    m_outputArray()
{}

//-------------------------------------------------------------------------------------------

CyanCodec::~CyanCodec()
{}

//-------------------------------------------------------------------------------------------

void CyanCodec::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "CyanCodec::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool CyanCodec::open(const QString& name)
{
    bool res = false;

    close();

    m_pDecoder = MusePackDecoderFactory::createShared("impl");
    if(!m_pDecoder.isNull())
    {
        res = m_pDecoder->open(name);
    }
    if(!res)
    {
        close();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void CyanCodec::close()
{
    if(!m_pDecoder.isNull())
    {
        m_pDecoder->close();
        m_pDecoder.clear();
    }
}

//-------------------------------------------------------------------------------------------

bool CyanCodec::decodeNextPacket(int& outLen)
{
    bool res = m_pDecoder->read(m_outputArray);
    outLen = static_cast<int>(m_outputArray.size() / (m_pDecoder->noChannels() * sizeof(tfloat32)));
    return res;
}

//-------------------------------------------------------------------------------------------

char *CyanCodec::getPacketBuffer()
{
    return m_outputArray.data();
}

//-------------------------------------------------------------------------------------------

int CyanCodec::bytesPerSample()
{
    return 4;
}

//-------------------------------------------------------------------------------------------

sample_t CyanCodec::readSample(char *buffer)
{
    tfloat32 *x = reinterpret_cast<tfloat32 *>(buffer);
    return static_cast<sample_t>(*x);
}

//-------------------------------------------------------------------------------------------

bool CyanCodec::isSeek() const
{
    return true;
}

//-------------------------------------------------------------------------------------------

bool CyanCodec::seek(const common::TimeStamp& v)
{
    bool res = false;

    if(!m_pDecoder.isNull() && m_pDecoder->seek(v))
    {
        m_state = 0;
        m_time = v;
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool CyanCodec::isComplete() const
{
    return false;
}

//-------------------------------------------------------------------------------------------

bool CyanCodec::isRemote() const
{
    return false;
}

//-------------------------------------------------------------------------------------------

bool CyanCodec::isBuffered(tfloat32& percent)
{
    return false;
}

//-------------------------------------------------------------------------------------------

tint CyanCodec::bitrate() const
{
    return (!m_pDecoder.isNull()) ? m_pDecoder->bitrate() : 0;
}

//-------------------------------------------------------------------------------------------

tint CyanCodec::frequency() const
{
    return (!m_pDecoder.isNull()) ? m_pDecoder->frequency() : 44100;
}

//-------------------------------------------------------------------------------------------

tint CyanCodec::noChannels() const
{
    return (!m_pDecoder.isNull()) ? m_pDecoder->noChannels() : 2;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp CyanCodec::length() const
{
    common::TimeStamp t(0);

    if(!m_pDecoder.isNull())
    {
        t = m_pDecoder->length();
    }
    return t;
}

//-------------------------------------------------------------------------------------------
} // namespace cyanomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
