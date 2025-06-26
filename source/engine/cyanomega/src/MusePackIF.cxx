#include "engine/cyanomega/inc/MusePackIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace cyanomega
{
//-------------------------------------------------------------------------------------------

MusePackIF::MusePackIF()
{}

//-------------------------------------------------------------------------------------------

MusePackIF::~MusePackIF()
{}

//-------------------------------------------------------------------------------------------

mpc_status MusePackIF::mpc_reader_init_stdio(mpc_reader *p_reader, const char *filename)
{
    return ::mpc_reader_init_stdio(p_reader,filename);
}

//-------------------------------------------------------------------------------------------

void MusePackIF::mpc_reader_exit_stdio(mpc_reader *p_reader)
{
    ::mpc_reader_exit_stdio(p_reader);
}

//-------------------------------------------------------------------------------------------

mpc_demux *MusePackIF::mpc_demux_init(mpc_reader *p_reader)
{
    return ::mpc_demux_init(p_reader);
}

//-------------------------------------------------------------------------------------------

void MusePackIF::mpc_demux_exit(mpc_demux *d)
{
    ::mpc_demux_exit(d);
}

//-------------------------------------------------------------------------------------------

void MusePackIF::mpc_demux_get_info(mpc_demux *d,mpc_streaminfo *i)
{
    ::mpc_demux_get_info(d,i);
}

//-------------------------------------------------------------------------------------------

mpc_status MusePackIF::mpc_demux_decode(mpc_demux *d,mpc_frame_info *i)
{
    return ::mpc_demux_decode(d,i);
}

//-------------------------------------------------------------------------------------------

mpc_int64_t MusePackIF::mpc_streaminfo_get_length_samples(mpc_streaminfo *si)
{
    return ::mpc_streaminfo_get_length_samples(si);
}

//-------------------------------------------------------------------------------------------

mpc_status MusePackIF::mpc_demux_seek_sample(mpc_demux *d,mpc_uint64_t destsample)
{
    return ::mpc_demux_seek_sample(d,destsample);
}

//-------------------------------------------------------------------------------------------

mpc_status MusePackIF::mpc_demux_seek_second(mpc_demux *d,double seconds)
{
    return ::mpc_demux_seek_second(d,seconds);
}

//-------------------------------------------------------------------------------------------

mpc_seek_t MusePackIF::mpc_demux_pos(mpc_demux *d)
{
    return ::mpc_demux_pos(d);
}

//-------------------------------------------------------------------------------------------
} // namespace cyanomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
