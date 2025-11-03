//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_GREENOMEGA_FLACMETASTREAMINFO_H
#define __OMEGA_GREENOMEGA_FLACMETASTREAMINFO_H
//-------------------------------------------------------------------------------------------

#include "engine/greenomega/inc/FLACMetaBase.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

class GREENOMEGA_EXPORT FLACMetaStreamInfo : public FLACMetaBase
{
    public:
        FLACMetaStreamInfo();
        virtual ~FLACMetaStreamInfo();

        virtual bool read(engine::Sequence *seq);

        common::TimeStamp length() const;

        tint m_minBlockSize;
        tint m_maxBlockSize;
        tint m_minFrameSize;
        tint m_maxFrameSize;
        tint m_frequency;
        tint m_channels;
        tint m_bitsPerSample;
        tuint64 m_totalSamples;
        tuint64 m_md5SignatureUpper;
        tuint64 m_md5SignatureLower;

    protected:

        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

