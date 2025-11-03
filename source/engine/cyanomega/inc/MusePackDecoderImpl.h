//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_CYANOMEGA_MUSEPACKDECODERIMPL_H
#define __OMEGA_ENGINE_CYANOMEGA_MUSEPACKDECODERIMPL_H
//-------------------------------------------------------------------------------------------

#include "engine/cyanomega/inc/MusePackIF.h"
#include "engine/cyanomega/inc/MusePackDecoder.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace cyanomega
{
//-------------------------------------------------------------------------------------------

class CYANOMEGA_EXPORT MusePackDecoderImpl : public MusePackDecoder
{
    public:
        MusePackDecoderImpl();
        virtual ~MusePackDecoderImpl();

        virtual bool open(const QString& fileName);
        virtual void close();

        virtual tint noChannels() const;
        virtual tint frequency() const;

        virtual bool read(QByteArray& mem);

        virtual tint bitrate() const;
        virtual common::TimeStamp length() const;

        virtual bool seek(const common::TimeStamp& v);

    protected:

        mpc_reader *m_reader;
        mpc_demux *m_demux;
        mpc_streaminfo *m_streamInfo;

        tfloat32 *m_sampleBuffer;

        QString m_name;
};

//-------------------------------------------------------------------------------------------
} // namespace cyanomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

