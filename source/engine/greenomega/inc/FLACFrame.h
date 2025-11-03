//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_GREENOMEGA_FLACFRAME_H
#define __OMEGA_ENGINE_GREENOMEGA_FLACFRAME_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Allocation.h"
#include "engine/inc/RData.h"
#include "engine/greenomega/inc/FLACHeader.h"
#include "engine/greenomega/inc/FLACSubframe.h"
#include "engine/greenomega/inc/FLACFramework.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

class GREENOMEGA_EXPORT FLACFrame
{
    public:
        FLACFrame(FLACMetaStreamInfo *info);
        virtual ~FLACFrame();

        bool read(Sequence *seq);

        const sample_t *left() const;
        const sample_t *right() const;
        const sample_t *output(int ch) const;

        const tint16 *leftInt16() const;
        const tint16 *rightInt16() const;
        const tint16 *outputInt16(int ch) const;

        const tint32 *leftInt32() const;
        const tint32 *rightInt32() const;
        const tint32 *outputInt32(int ch) const;

        tint length() const;
        tint noChannels() const;

        const common::TimeStamp& start() const;
        const common::TimeStamp& end() const;

        bool seek(FLACFramework *framework,common::TimeStamp& t);

        virtual void setDataTypeFormat(CodecDataType type);
        virtual CodecDataType dataType() const;

    protected:

        FLACMetaStreamInfo *m_streamInfo;
        FLACHeader m_header;
        FLACSubframe *m_subframe[8];

        // Buffer for 64-bit double precision
        tint m_outSize;
        sample_t *m_out[8];
        // Buffer for 16-bit integer
        tint m_outSizeInt16;
        tint16 *m_outInt16[8];
        // Buffer for 24-bit OR 32-bit integer
        tint m_outSizeInt32;
        tint32 *m_outInt32[8];

        common::TimeStamp m_timeStart;
        common::TimeStamp m_timeEnd;

        tint m_count;

        CodecDataType m_outputFormatType;

        bool allocate();
        bool crc(Sequence *seq);

        void processOutput();
        void processOutputFloat(tint size);
        void processOutputInt16(tint size);
        void processOutputInt24(tint size);
        void processOutputInt32(tint size);
};

//-------------------------------------------------------------------------------------------

inline const sample_t *FLACFrame::output(int ch) const
{
    return m_out[ch];
}

//-------------------------------------------------------------------------------------------

inline tint FLACFrame::noChannels() const
{
    return m_header.channels();
}

//-------------------------------------------------------------------------------------------

inline const sample_t *FLACFrame::left() const
{
    return m_out[0];
}

//-------------------------------------------------------------------------------------------

inline const sample_t *FLACFrame::right() const
{
    return m_out[1];
}

//-------------------------------------------------------------------------------------------

inline const tint16 *FLACFrame::leftInt16() const
{
    return m_outInt16[0];
}

//-------------------------------------------------------------------------------------------

inline const tint16 *FLACFrame::rightInt16() const
{
    return m_outInt16[1];
}

//-------------------------------------------------------------------------------------------

inline const tint16 *FLACFrame::outputInt16(int ch) const
{
    return m_outInt16[ch];
}

//-------------------------------------------------------------------------------------------

inline const tint32 *FLACFrame::leftInt32() const
{
    return m_outInt32[0];
}

//-------------------------------------------------------------------------------------------

inline const tint32 *FLACFrame::rightInt32() const
{
    return m_outInt32[1];
}

//-------------------------------------------------------------------------------------------

inline const tint32 *FLACFrame::outputInt32(int ch) const
{
    return m_outInt32[ch];
}

//-------------------------------------------------------------------------------------------

inline tint FLACFrame::length() const
{
    return m_header.blockSize();
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& FLACFrame::start() const
{
    return m_timeStart;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& FLACFrame::end() const
{
    return m_timeEnd;
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

