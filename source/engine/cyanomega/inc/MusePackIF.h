//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_CYANOMEGA_MUSEPACKIF_H
#define __OMEGA_ENGINE_CYANOMEGA_MUSEPACKIF_H
//-------------------------------------------------------------------------------------------

#include "engine/cyanomega/inc/CyanOmegaDLL.h"

#define __CYANOMEGA

#include "mpc/reader.h"
#include "mpc/mpcdec.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace cyanomega
{
//-------------------------------------------------------------------------------------------

class CYANOMEGA_EXPORT MusePackIF
{
    public:
        MusePackIF();
        virtual ~MusePackIF();

        virtual mpc_status mpc_reader_init_stdio(mpc_reader *p_reader, const char *filename);
        virtual void mpc_reader_exit_stdio(mpc_reader *p_reader);

        virtual mpc_demux *mpc_demux_init(mpc_reader *p_reader);
        virtual void mpc_demux_exit(mpc_demux *d);

        virtual void mpc_demux_get_info(mpc_demux *d,mpc_streaminfo *i);

        virtual mpc_status mpc_demux_decode(mpc_demux *d,mpc_frame_info *i);

        virtual mpc_int64_t mpc_streaminfo_get_length_samples(mpc_streaminfo *si);

        virtual mpc_status mpc_demux_seek_sample(mpc_demux *d,mpc_uint64_t destsample);
        virtual mpc_status mpc_demux_seek_second(mpc_demux *d,double seconds);

        virtual mpc_seek_t mpc_demux_pos(mpc_demux *d);
};

//-------------------------------------------------------------------------------------------
} // namespace cyanomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

