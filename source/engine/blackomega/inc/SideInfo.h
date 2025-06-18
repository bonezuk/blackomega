//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BLACKOMEGA_SIDEINFO_H
#define __OMEGA_ENGINE_BLACKOMEGA_SIDEINFO_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/BitStream.h"
#include "engine/inc/Sequence.h"
#include "engine/blackomega/inc/BandIndex.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

#define SBLIMIT 32
#define SSLIMIT 18

#define MPG_MD_STEREO 0
#define MPG_MD_JOINT_STEREO 1
#define MPG_MD_DUAL_CHANNEL 2
#define MPG_MD_MONO 3

#define MPEG_AUDIO_ID 1
#define MPEG_PHASE2_LSF 0

//-------------------------------------------------------------------------------------------

typedef struct
{
    tint mpeg_25;
    tint lsf;
    tint layer;
    tint error_protection;
    tint bitrate_index;
    tint sampling_frequency;
    tint padding;
    tint extension;
    tint mode;
    tint mode_ext;
    tint copyright;
    tint original;
    tint emphasis;
    tuint sfreq;
    tuint stereo;
} MPHeaderInfo;

//-------------------------------------------------------------------------------------------

typedef struct
{
    tuint part2_3_length;
    tuint big_values;
    tuint global_gain;
    tuint scalefac_compress;
    tuint block_type;
    tuint mixed_block_flag;
    tuint table_select[3];
    tuint subblock_gain[3];
    tuint region1Start;
    tuint region2Start;
    tuint preflag;
    tuint scalefac_scale;
    tuint count1table_select;
} GrInfoS;

//-------------------------------------------------------------------------------------------

typedef struct
{
    tuint scfsi[4];
    GrInfoS gr[2];
} ChannelInfoS;

//-------------------------------------------------------------------------------------------

typedef struct
{
    tuint main_data_begin;
    tuint private_bits;
    ChannelInfoS ch[2];
} SideInfoS;

//-------------------------------------------------------------------------------------------

typedef struct
{
    tint l[23];
    tint s[3][13];
} ScaleInfoS;

//-------------------------------------------------------------------------------------------

class BLACKOMEGA_EXPORT SideInfo
{
    public:
        SideInfo();
        
        virtual void *get(tint val = 0);
        
        bool read(Sequence *seq);
        
        void set(MPHeaderInfo *p);
        void set(SideInfoS *p);
        
    protected:
    
        MPHeaderInfo *m_hdr;
        SideInfoS *m_info;
        
        bool readInfoMPEG1(Sequence *seq);
        bool readInfoMPEG2(Sequence *seq);
};

//-------------------------------------------------------------------------------------------

inline void SideInfo::set(MPHeaderInfo *p)
{
    m_hdr = p;
}

//-------------------------------------------------------------------------------------------

inline void SideInfo::set(SideInfoS *p)
{
    m_info = p;
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

