//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BLACKOMEGA_MPHEADER_H
#define __OMEGA_ENGINE_BLACKOMEGA_MPHEADER_H
//-------------------------------------------------------------------------------------------

#include "common/inc/TimeStamp.h"
#include "engine/inc/BitStream.h"
#include "engine/blackomega/inc/BSequence.h"
#include "engine/blackomega/inc/BandIndex.h"
#include "engine/blackomega/inc/SideInfo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

#define FRAMES_FLAG    0x00000001
#define BYTES_FLAG     0x00000002
#define TOC_FLAG       0x00000004
#define VBR_SCALE_FLAG 0x00000008

//-------------------------------------------------------------------------------------------

typedef struct
{
    tint h_id;            // from MPEG header, 0=MPEG2, 1=MPEG1
    tint samprate;        // determined from MPEG header
    tint flags;            // from Xing header data
    tint frames;        // total bit stream frames from Xing header data
    tint bytes;            // total bit stream bytes from Xing header data
    tint vbr_scale;        // encoded vbr scale from Xing header data
    tubyte *toc;        // point to toc_buffer[100] or maybe null if toc not desired
} XHeadData;

//-------------------------------------------------------------------------------------------

class HeaderItem
{
    public:
        HeaderItem();

        HeaderItem *m_prev;
        HeaderItem *m_next;
        tint m_headWidth;
        tint m_dataWidth;
        tint m_headBookmark;
        tint m_bodyBookmark;
        tint m_freeBookmark;
        MPHeaderInfo m_header;
        SideInfoS m_sideinfo;

        void clear();
};

//-------------------------------------------------------------------------------------------

class BLACKOMEGA_EXPORT MPHeader
{
    public:
        MPHeader();
        ~MPHeader();

        void set(engine::Bitstream *bs);

        bool start();
        bool seek(common::TimeStamp& t,tint& frames);

        HeaderItem *next();
        void complete(HeaderItem *item);

        tint bitrate() const;
        tint frequency() const;
        const common::TimeStamp& length() const;

    protected:

        common::Allocation m_alloc;
        engine::Bitstream *m_bs;

        HeaderItem *m_firstItem;
        HeaderItem *m_lastItem;
        HeaderItem *m_freeItem;

        tint m_headerBookmark;

        tint m_startOffset;
        tint m_frequencyIndex;
        tint m_frequency;
        tint m_kbps;
        bool m_stereo;
        tint m_samplesPerSecond;
        tint m_width;
        tint m_widthHeader;
        tint m_widthData;
        tfloat64 m_spf;
        XHeadData *m_xingHeader;
        common::TimeStamp m_length;
        tint m_rate;

        HeaderItem *getHeaderItem();
        void freeHeaderItem(HeaderItem *item);

        bool validate(tuint h);
        void decodeHeader(MPHeaderInfo *hdr,tuint h);

        tint getBitrate(MPHeaderInfo *info);

        tint width(MPHeaderInfo *hdr);
        tint width(MPHeaderInfo *hdr,tint& hdrSize,tint& dataSize);

        bool scour(Sequence *seq,tint& offset);
        bool sequenceSearch(Sequence *seq,tint& offset,bool nFlag);

        void readXingInfo(Sequence *seq,MPHeaderInfo *hdr);
};

//-------------------------------------------------------------------------------------------

inline void MPHeader::set(engine::Bitstream *bs)
{
    m_bs = bs;
}

//-------------------------------------------------------------------------------------------

inline tint MPHeader::frequency() const
{
    return m_frequency;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& MPHeader::length() const
{
    return m_length;
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

