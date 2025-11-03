#include "engine/cyanomega/inc/MusePackDecoderImpl.h"
#if defined(OMEGA_MAC_STORE)
#include "common/inc/SBService.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace cyanomega
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(MusePackDecoderFactory,MusePackDecoder, \
                            MusePackDecoderImplFactory,MusePackDecoderImpl, \
                            "impl",false)

//-------------------------------------------------------------------------------------------

MusePackDecoderImpl::MusePackDecoderImpl() : MusePackDecoder(),
    m_reader(0),
    m_demux(0),
    m_streamInfo(0),
    m_sampleBuffer(0),
    m_name()
{}

//-------------------------------------------------------------------------------------------

MusePackDecoderImpl::~MusePackDecoderImpl()
{}

//-------------------------------------------------------------------------------------------

bool MusePackDecoderImpl::open(const QString& fileName)
{
    MusePackIF mpcIF;
    mpc_status err;
    bool res;

    close();

    m_name = fileName;
#if defined(OMEGA_MAC_STORE)
    common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
    if(sbBookmark->has(fileName,true))
    {
        sbBookmark->checkOut(fileName,true);
    }
#endif

    m_reader = new mpc_reader;
    if(m_reader!=0)
    {
        ::memset(m_reader,0,sizeof(mpc_reader));
        err = mpcIF.mpc_reader_init_stdio(m_reader,fileName.toUtf8().constData());
        if(err == MPC_STATUS_OK)
        {
            m_demux = mpcIF.mpc_demux_init(m_reader);
            if(m_demux!=0)
            {
                m_streamInfo = reinterpret_cast<mpc_streaminfo *>(::malloc(sizeof(mpc_streaminfo)));
                if(m_streamInfo!=0)
                {
                    mpcIF.mpc_demux_get_info(m_demux,m_streamInfo);
                    m_sampleBuffer = reinterpret_cast<tfloat32 *>(::malloc(MPC_DECODER_BUFFER_LENGTH * sizeof(tfloat32)));
                    res = true;
                }
                else
                {
                    res = false;
                }
            }
            else
            {
                res = false;
            }
        }
        else
        {
            ::free(m_reader);
            m_reader = 0;
            res = false;
        }
    }
    else
    {
        res = false;
    }

    if(!res)
    {
        close();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void MusePackDecoderImpl::close()
{
    MusePackIF mpcIF;

    if(m_sampleBuffer!=0)
    {
        ::free(m_sampleBuffer);
        m_sampleBuffer = 0;
    }
    if(m_streamInfo!=0)
    {
        ::free(m_streamInfo);
        m_streamInfo = 0;
    }
    if(m_demux!=0)
    {
        mpcIF.mpc_demux_exit(m_demux);
        m_demux = 0;
    }
    if(m_reader!=0)
    {
        mpcIF.mpc_reader_exit_stdio(m_reader);
        delete m_reader;
        m_reader = 0;
    }

#if defined(OMEGA_MAC_STORE)
    common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
    sbBookmark->checkIn(m_name,true);
#endif
}

//-------------------------------------------------------------------------------------------

tint MusePackDecoderImpl::noChannels() const
{
    return (m_streamInfo!=0) ? m_streamInfo->channels : 2;
}

//-------------------------------------------------------------------------------------------

tint MusePackDecoderImpl::frequency() const
{
    return (m_streamInfo!=0) ? m_streamInfo->sample_freq : 44100;
}

//-------------------------------------------------------------------------------------------

bool MusePackDecoderImpl::read(QByteArray& mem)
{
    MusePackIF mpcIF;
    mpc_status err;
    mpc_frame_info frame;
    bool res;

    ::memset(&frame,0,sizeof(mpc_frame_info));
    ::memset(m_sampleBuffer,0,MPC_DECODER_BUFFER_LENGTH * sizeof(tfloat32));

    frame.buffer = m_sampleBuffer;

    err = mpcIF.mpc_demux_decode(m_demux,&frame);

    if(err==MPC_STATUS_OK && frame.bits!=-1)
    {
        mem.clear();
        mem.append(reinterpret_cast<const char *>(m_sampleBuffer),frame.samples * noChannels() * sizeof(tfloat32));
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp MusePackDecoderImpl::length() const
{
    tfloat64 len;

    if(m_streamInfo!=0)
    {
        MusePackIF mpcIF;
        len = static_cast<tfloat64>(mpcIF.mpc_streaminfo_get_length_samples(m_streamInfo)) / static_cast<tfloat64>(frequency());
    }
    else
    {
        len = 0.0;
    }
    return common::TimeStamp(len);
}

//-------------------------------------------------------------------------------------------

bool MusePackDecoderImpl::seek(const common::TimeStamp& v)
{
    MusePackIF mpcIF;
    bool res = false;

    if(m_demux!=0)
    {
        if(mpcIF.mpc_demux_seek_second(m_demux,static_cast<tfloat64>(v))==MPC_STATUS_OK)
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint MusePackDecoderImpl::bitrate() const
{
    tint rate = 0;

    if(m_streamInfo!=0)
    {
        rate = static_cast<tint>(m_streamInfo->average_bitrate);
    }
    return rate;
}

//-------------------------------------------------------------------------------------------
} // namespace cyanomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
